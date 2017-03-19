#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include <float.h>

#include "resource.h"

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_mixer.h"
#include "SDL2/SDL2_gfxPrimitives.h"
#include "SDL2/SDL2_sgfxPrimitives.h"

#define  XTRACE_REPLACE
#include "xtrace.h"
#include "datatypes.h"
#define  COLORMODE_ARGB8888
#include "colors.h"
#include "timer.h"

#include "mathfuncs.h"

internal const char *musicFileName = "../res/res.wav";

// ======== GLOBALS ======== //
global bool Global_running = true;
global bool Global_paused = true;
global SDL_Renderer *renderer;
global const int Window_Width  = 1920;
global const int Window_Height = 1080;
global const int Half_WW = Window_Width / 2;
global const int Fourth_WW = Window_Width / 4;
global const int Half_WH = Window_Height / 2;
global const int Fourth_WH = Window_Height / 4;
global const int BORDER = 10;
global int LENGTH = 9;
global const int INCREMENT = 8;
global bool CLEAR = true;
global int REFRATE = 1;
global int VOLUME = SDL_MIX_MAXVOLUME;
global bool DELAY = false;
global float32 cursor = 10.0f;
global float64 zoom = 1.0f;
global int hh = Window_Height / 2;
global int startl = hh - (hh / 2);
global int startr = Window_Height - (hh / 2);
global SDL_Surface *bufferSurface;
// ========================= //

// ======== CUSTOM COLORS ======== //
global const uint32 COLOR_BACKGROUNDC  = 0xFF1F1F1F; // Dark grey
global const uint32 COLOR_LIGHTLINE    = 0xFF4A4A4A; // Light grey
// ============================== //

#include "wav.h"
#include "handleevents.h"
#include "drawing.h"

View Global_View = VIEW_WAVEFORM;

struct VUMeter
{
  uint32  COLOR_AVG  = 0x00FF00FF;
  uint32  COLOR_PEAK = 0x009600FF;
  float32 avgL  = 0.0f;
  float32 avgR  = 0.0f;
  float32 peakL = 0.0f;
  float32 peakR = 0.0f;
};

VUMeter *createVUMeter()
{
  VUMeter *result = (VUMeter *)malloc(sizeof(VUMeter));
  result->COLOR_AVG  = 0xFF00FF00;
  result->COLOR_PEAK = 0xFF009600;
  result->avgL  = 0.0f;
  result->avgR  = 0.0f;
  result->peakL = 0.0f;
  result->peakR = 0.0f;
  return result;
}

void AudioPostMix(void *udata, uint8 *stream, int len)
{
  float32 suml = 0.0f;
  float32 sumr = 0.0f;
  float32 maxl = FLT_MIN;
  float32 maxr = FLT_MIN;

  SDL_FillRect(bufferSurface, NULL, 0x00000000);

  for(int i = 0, x = 0; i < len; ++i, x += 4)
  {
    int16 l = (stream[i] << 8) | (stream[++i] & 0xFF);
    int16 r = (stream[++i] << 8) | (stream[++i] & 0xFF);
    int16 ll = abs(l);
    int16 rr = abs(r);
    float32 la = ll / (float32)WAV_MAXSAMPLE;
    float32 ra = rr / (float32)WAV_MAXSAMPLE;
    suml += (la * la);
    sumr += (ra * ra);

    maxl = fminf(l, maxl);
    maxr = fminf(r, maxr);

    svlineColor(bufferSurface, x, Fourth_WH, l / Fourth_WH, 0x80FF80FF);
  }
  maxl = (maxl * maxl) / WAV_MAXSAMPLE_F;
  maxr = (maxr * maxr) / WAV_MAXSAMPLE_F;

  float32 rmsal = sqrt(suml / len);
  float32 rmsar = sqrt(sumr / len);
  float32 dbal = 20 * log10(rmsal);
  float32 dbar = 20 * log10(rmsar);
  float32 rmspl = sqrt(maxl / WAV_MAXSAMPLE_F);
  float32 rmspr = sqrt(maxr / WAV_MAXSAMPLE_F);
  float32 dbpl = 20 * log10(rmspl);
  float32 dbpr = 20 * log10(rmspr);

  float32 vuAvgL  = exp(dbal * (log10(10.0f) / 20.0f));
  float32 vuAvgR  = exp(dbar * (log10(10.0f) / 20.0f));
  float32 vuPeakL = exp(dbpl * (log10(10.0f) / 20.0f));
  float32 vuPeakR = exp(dbpr * (log10(10.0f) / 20.0f));

  ((VUMeter *)udata)->avgL  = vuAvgL;
  ((VUMeter *)udata)->avgR  = vuAvgR;
  ((VUMeter *)udata)->peakL = vuPeakL;
  ((VUMeter *)udata)->peakR = vuPeakR;

  #if 0
  // DEBUG
  if(dbal > -96 && dbar > -96)
  {
    printf("   DB AVG | DB PEAK | VU AVG | VU PEAK\n");
    printf("L: %6.2f |%8.2f |%7.2f |%8.2f\n", dbal, dbpl, vuAvgL, vuPeakL);
    printf("R: %6.2f |%8.2f |%7.2f |%8.2f\n", dbal, dbpl, vuAvgR, vuPeakR);
    printf("\n");
  }
  #endif
}

int main(int argc, char **argv)
{
  printf("Hello world\n\n");

  srand((unsigned)time(NULL));

  // -----------------------------------------------------------------------------------------------
  if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0)
  {
    printf("Error initializing SDL. Exiting.\n");
    printf("%s\n", SDL_GetError());
    return -1;
  }
  atexit(SDL_Quit);

  // -----------------------------------------------------------------------------------------------
  uint32 windowFlags = 0;
  // windowFlags |= SDL_WINDOW_FULLSCREEN;
  SDL_Window *window = SDL_CreateWindow("Hex", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                        Window_Width, Window_Height, windowFlags);
  if(!window)
  {
    printf("Could not create window. Exiting.\n");
    printf("%s\n", SDL_GetError());
    return -1;
  }
  IMG_Init(0);
  SDL_RWops *rwop;
  rwop = SDL_RWFromFile("../res/hex.ico", "rb");
  SDL_Surface *icon = IMG_LoadICO_RW(rwop);
  SDL_SetWindowIcon(window, icon);
  SDL_FreeSurface(icon);

  // -----------------------------------------------------------------------------------------------
  // SDL_Renderer *renderer = SDL_CreateSoftwareRenderer(windowSurface);
  int rendererFlags = SDL_RENDERER_ACCELERATED;
  //rendererFlags |= SDL_RENDERER_PRESENTVSYNC;
  renderer = SDL_CreateRenderer(window, -1, rendererFlags);

  if(!renderer)
  {
    printf("Could not create renderer. Exiting.\n");
    printf("%s\n", SDL_GetError());
    return -1;
  }

  // PRINT RENDERER INFORMATION
  #if 0
  SDL_RendererInfo rinfo;
  SDL_GetRendererInfo(renderer, &rinfo);
  printf("Video:\nRenderer name: %s\n", rinfo.name);
  printf("Texture formats: \n");
  for(int i = 0; i < rinfo.num_texture_formats-1; ++i)
  {
    printf("\t%s,\n", SDL_GetPixelFormatName(rinfo.texture_formats[i]));
  }
  printf("\t%s\n", SDL_GetPixelFormatName(rinfo.texture_formats[rinfo.num_texture_formats-1]));
  printf("End video\n\n");
  #endif

  // -----------------------------------------------------------------------------------------------
  Mix_Init(0);

  if(Mix_OpenAudio(48000, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
  {
    printf("Failed to open SDL_Mixer: %s\nExiting.\n", Mix_GetError());
    return -1;
  }

  // -----------------------------------------------------------------------------------------------
  WAVFile wavFile = WAV_openFile(musicFileName);
  if(!wavFile.data)
  {
    printf("No wav data. Exiting\n");
    return -1;
  }
  printWAVFile(wavFile);
  SDL_Texture *wavTexture = WAV_createTexture(renderer, wavFile, Window_Height);
  SDL_SetTextureBlendMode(wavTexture, SDL_BLENDMODE_BLEND);
  WAV_closeFile(wavFile);

  // -----------------------------------------------------------------------------------------------
  Mix_Music *music = Mix_LoadMUS(musicFileName);
  if(!music)
  {
    printf("Could not load music.\n%s\nExiting.\n", Mix_GetError());
    return -1;
  }

  int audio_channels = 0, audio_rate = 0;
  Uint16 audio_format;
  Mix_QuerySpec(&audio_rate, &audio_format, &audio_channels);
  // PRINT AUDIO INFORMATION
  #if 0
  char *format_str="Unknown";
  switch(audio_format)
  {
    case AUDIO_U8: format_str="U8"; break;
    case AUDIO_S8: format_str="S8"; break;
    case AUDIO_U16LSB: format_str="U16LSB"; break;
    case AUDIO_S16LSB: format_str="S16LSB"; break;
    case AUDIO_U16MSB: format_str="U16MSB"; break;
    case AUDIO_S16MSB: format_str="S16MSB"; break;
  }
  printf("Audio:\n\tSample rate: %d Hz\n\tFormat: %s\n\tChannels: %d\n\tBits: %d\n", 
         audio_rate, format_str, audio_channels, bits);
  printf("End Audio\n\n");
  #endif
  VUMeter *vuMeter = createVUMeter();
  Mix_SetPostMix(AudioPostMix, vuMeter);

  // -----------------------------------------------------------------------------------------------
  TTF_Init();
  if(!TTF_WasInit())
  {
    printf("Could not init TTF. Exiting.\n");
    printf("%s\n", TTF_GetError());
    return -1;
  }
  TTF_Font *font16 = TTF_OpenFont("../res/consolas.ttf", 16);

  // -----------------------------------------------------------------------------------------------
  Mix_VolumeMusic(VOLUME);
  #if 1
  if(Mix_PlayMusic(music, -1) == -1)
  {
    printf("Failed to play music.\n");
    return -1;
  }
  #endif
  Mix_PauseMusic();
  
  // -----------------------------------------------------------------------------------------------
  Mouse mouse;
  
  const int DB = 100;
  const int zeroDB = (Window_Height - DB);
  const int boxw = 16;
  const int leftx = Window_Width - 100;
  const int rightx = leftx + boxw + 4;

  int w, h;
  SDL_QueryTexture(wavTexture, NULL, NULL, &w, &h);
  SDL_Rect wavRect = {10, 0, leftx - 20, h};

  int linex = leftx + (boxw * 2) + 5;

  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

  SDL_Color SDL_COLOR_LIGHTGREY = { 0x4A, 0x4A, 0x4A, 0xFF };
  SDL_Texture *sixDBText = createTextTexture(renderer, "6.0", font16, SDL_COLOR_LIGHTGREY);
  SDL_Texture *zeroDBText = createTextTexture(renderer, "0.0", font16, SDL_COLOR_LIGHTGREY);
  SDL_Texture *nsixDBText = createTextTexture(renderer, "-6.0", font16, SDL_COLOR_LIGHTGREY);
  SDL_Texture *ntwelveDBText = createTextTexture(renderer, "-12.0", font16, SDL_COLOR_LIGHTGREY);
  SDL_Texture *neighteenDBText = createTextTexture(renderer, "-18.0", font16, SDL_COLOR_LIGHTGREY);
  SDL_Texture *ntwentyfourDBText = createTextTexture(renderer, "-24.0", font16, SDL_COLOR_LIGHTGREY);

  bufferSurface = SDL_CreateRGBSurface(0, 1080, 1080, 32, 
                                       0xFF000000,
                                       0x00FF0000,
                                       0x0000FF00,
                                       0x000000FF);

  SDL_Texture *bufferTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, 
                                    SDL_TEXTUREACCESS_STREAMING, wavRect.w, wavRect.h);

  // -----------------------------------------------------------------------------------------------
  do
  {
    HandleEvents(renderer, window, &mouse, music, &cursor, &Global_View, wavTexture, &wavRect);
    clear(renderer, COLOR_BACKGROUNDC);

    if(Global_View == VIEW_WAVEFORM) SDL_RenderCopy(renderer, wavTexture, NULL, &wavRect);
    else if(Global_View == VIEW_BUFFER)
    {
      SDL_UpdateTexture(bufferTexture, NULL, bufferSurface->pixels, bufferSurface->pitch);
      SDL_RenderCopy(renderer, bufferTexture, NULL, &wavRect);
    }

    // ==== VU ==== //
    boxColor(renderer, leftx, 0, leftx + boxw, Window_Height, COLOR_BLACK);
    boxColor(renderer, rightx, 0, rightx + boxw, Window_Height, COLOR_BLACK);
    hlineColor(renderer, linex, linex + 5, 0.25 * Window_Height, COLOR_LIGHTLINE);
    drawTextToRenderer(renderer, linex + 10, 0.25 * Window_Height, "-6.0", font16, SDL_COLOR_LIGHTGREY);
    hlineColor(renderer, linex, linex + 5, 0.5 * Window_Height,  COLOR_LIGHTLINE);
    hlineColor(renderer, linex, linex + 5, 0.75 * Window_Height, COLOR_LIGHTLINE);
    // ==== PEAK ==== //
    int peakL = vuMeter->peakL * zeroDB;
    int peakR = vuMeter->peakR * zeroDB;
    boxColor(renderer, leftx, Window_Height, leftx + boxw, Window_Height - peakL, vuMeter->COLOR_PEAK);
    boxColor(renderer, rightx, Window_Height, rightx + boxw, Window_Height - peakR, vuMeter->COLOR_PEAK);
    // ==== AVG ==== //
    int avgL = vuMeter->avgL * zeroDB;
    int avgR = vuMeter->avgR * zeroDB;
    boxColor(renderer, leftx, Window_Height, leftx + boxw, Window_Height - avgL, vuMeter->COLOR_AVG);
    boxColor(renderer, rightx, Window_Height, rightx + boxw, Window_Height - avgR, vuMeter->COLOR_AVG);
    
    hlineColor(renderer, leftx, leftx + boxw, DB, COLOR_LIGHTLINE);
    hlineColor(renderer, rightx, rightx + boxw, DB, COLOR_LIGHTLINE);

    // TODO(alex): Make the cursor move at the actual rate of the song.
    if(!Mix_PausedMusic() && Mix_PlayingMusic()) 
      cursor += (wavFile.duration / (float32)Window_Width - 50);
    if(cursor >= (Window_Width - 50)) cursor = 10.0f;

    SDL_RenderPresent(renderer);
  } while(Global_running);
  // -----------------------------------------------------------------------------------------------

  free(vuMeter);

  IMG_Quit();
  TTF_CloseFont(font16);
  Mix_CloseAudio();
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  printf("Goodbye.\n");

  return 0;
}