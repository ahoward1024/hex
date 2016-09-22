#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include <float.h>

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
// ========================= //

// ======== CUSTOM COLORS ======== //
global const uint32 COLOR_BACKGROUNDC  = 0xFF1F1F1F; // Dark grey
global const uint32 COLOR_LIGHTLINE    = 0xFF4A4A4A; // Light grey
global const uint32 COLOR_VUAVG        = 0xFF00FF00; // Bright green
global const uint32 COLOR_VUPEAK       = 0xFF009600; // Dark green
// ============================== //

// ======== VU METER ======== //
internal float32 vuAvgL   = 0.0f;
internal float32 vuAvgR   = 0.0f;
internal float32 vuPeakL  = 0.0f;
internal float32 vuPeakR  = 0.0f;
// ========================= //

internal int16 maxSampleL = SHRT_MAX;
internal int16 maxSampleR = SHRT_MAX;

#include "wave.h"
#include "handleevents.h"

void AudioPostMix(void *udata, uint8 *stream, int len)
{
  float64 suml = 0.0;
  float64 sumr = 0.0;
  float64 maxl = FLT_MIN;
  float64 maxr = FLT_MIN;
  for(int i = 0; i < len; ++i)
  {
    int16 l = abs((stream[i] << 8) | (stream[++i]));
    int16 r = abs((stream[++i] << 8) | (stream[++i]));
    float32 la = l / (float32)maxSampleL;
    float32 ra = r / (float32)maxSampleR;
    suml += (la * la);
    sumr += (ra * ra);
    maxl = maximum(l, maxl);
    maxr = maximum(r, maxr);
  }
  maxl = (maxl * maxl) / 32768.0;
  maxr = (maxr * maxr) / 32768.0;

  float32 rmsal = sqrt(suml / len);
  float32 rmsar = sqrt(sumr / len);
  float32 dbal  = 20 * log10(rmsal);
  float32 dbar  = 20 * log10(rmsar);
  float32 rmspl = sqrt(maxl / 32768.0);
  float32 rmspr = sqrt(maxr / 32768.0);
  float32 dbpl  = 20 * log10(rmspl);
  float32 dbpr  = 20 * log10(rmspr);
  
  #if 1
  vuAvgL  = exp(dbal * (log(10.0f) / 20.0f));
  vuAvgR  = exp(dbar * (log(10.0f) / 20.0f));
  vuPeakL = exp(dbpl * (log(10.0f) / 20.0f));
  vuPeakR = exp(dbpr * (log(10.0f) / 20.0f));
  #endif
  #if 0
  // DEBUG
  if(dbal > -96 && dbar > -96)
  {
    printf("   DB AVG | DB PEAK | VU AVG | VU PEAK | VU POW | VU DYN\n");
    printf("L: %6.2f |%8.2f |%7.2f |%8.2f |%7.2f |%7.2f\n", dbal, dbpl, vuAvgL, vuPeakL, vuPowL, vuDynL);
    printf("R: %6.2f |%8.2f |%7.2f |%8.2f |%7.2f |%7.2f\n", dbal, dbpl, vuAvgR, vuPeakR, vuPowR, vuDynR);
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
  rendererFlags |= SDL_RENDERER_PRESENTVSYNC;
  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, rendererFlags);

  if(!renderer)
  {
    printf("Could not create renderer. Exiting.\n");
    printf("%s\n", SDL_GetError());
    return -1;
  }

  // PRINT RENDERER INFORMATION
  #if 1
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
  printWAVFile(wavFile);
  SDL_Texture *wavTexture = WAV_createTexture(renderer, wavFile, Window_Height - 20);
  SDL_SetTextureBlendMode(wavTexture, SDL_BLENDMODE_BLEND);
  maxSampleL = wavFile.maxLeft;
  maxSampleR = wavFile.maxRight;
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
  Mix_SetPostMix(AudioPostMix, NULL);

  // -----------------------------------------------------------------------------------------------
  TTF_Init();
  if(!TTF_WasInit())
  {
    printf("Could not init TTF. Exiting.\n");
    printf("%s\n", TTF_GetError());
    return -1;
  }
  TTF_Font *fontConsolas24 = TTF_OpenFont("../res/consolas.ttf", 24);

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
  
  Mouse mouse;
  int w, h;
  SDL_QueryTexture(wavTexture, NULL, NULL, &w, &h);
  SDL_Rect wavRect = {10, 10, Window_Width - 100, h};
  const int DB = 100;
  const int zeroDB = (Window_Height - DB);
  const int boxw = 16;
  const int leftx = Window_Width - 36;
  const int rightx = (Window_Width - 36) + boxw + 4;

  do
  {
    HandleEvents(renderer, window, &mouse, music, &cursor, wavTexture, &wavRect);
    SDL_SetRenderDrawColor(renderer, 31, 31, 31, 255);
    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, wavTexture, NULL, &wavRect);

    // ==== VU ==== //
    boxColor(renderer, leftx, 0, leftx + boxw, Window_Height, COLOR_BLACK);
    boxColor(renderer, rightx, 0, rightx + boxw, Window_Height, COLOR_BLACK);
    // ==== PEAK ==== //
    boxColor(renderer, leftx, Window_Height, leftx + boxw, Window_Height - (vuPeakL * zeroDB), COLOR_VUPEAK);
    boxColor(renderer, rightx, Window_Height, rightx + boxw, Window_Height - (vuPeakR * zeroDB), COLOR_VUPEAK);
    // ==== AVG ==== //
    boxColor(renderer, leftx, Window_Height, leftx + boxw, Window_Height - (vuAvgL * zeroDB), COLOR_VUAVG);
    boxColor(renderer, rightx, Window_Height, rightx + boxw, Window_Height - (vuAvgR * zeroDB), COLOR_VUAVG);
    
    hlineColor(renderer, leftx, leftx + boxw, DB, COLOR_LIGHTLINE);
    hlineColor(renderer, rightx, rightx + boxw, DB, COLOR_LIGHTLINE);

    // TODO(alex): Make the cursor move at the actual rate of the song.
    if(!Mix_PausedMusic() && Mix_PlayingMusic()) 
      cursor += (wavFile.duration / (float32)Window_Width - 50);
    if(cursor >= (Window_Width - 50)) cursor = 10.0f;

    SDL_RenderPresent(renderer);
  } while(Global_running);

  IMG_Quit();
  TTF_CloseFont(fontConsolas24);
  Mix_CloseAudio();
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  printf("Goodbye.\n");

  return 0;
}