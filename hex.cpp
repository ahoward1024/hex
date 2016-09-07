#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include <float.h>


#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_mixer.h"
#include "SDL/SDL_gfxPrimitives.h"

#define XTRACE_REPLACE
#include "xtrace.h"
#include "datatypes.h"
#define  COLORMODE_ARGB8888
#include "colors.h"
#include "timer.h"
#include "drawing.h"

#include "resource.h"

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

internal const char *musicFileName = "../res/res.wav";

global const uint32 COLOR_BACKGROUNDC  = 0xFF1F1F1F; // Dark grey
global const uint32 COLOR_LEFTCHANNEL  = 0xFF80FF80; // Light green
global const uint32 COLOR_RIGHTCHANNEL = 0xFFFF8080; // Light red
global const uint32 COLOR_LIGHTLINE    = 0xFF4A4A4A; // Light grey
global const uint32 COLOR_VUAVERAGE    = 0xFF00FF00; // Bright green
global const uint32 COLOR_VUPEAK       = 0xFF009600; // Dark green

internal float32 vuAvgLeft = 0;
internal float32 vuAvgRight = 0;
internal float32 vuPeakLeft = 0;
internal float32 vuPeakRight = 0;

internal int16 maxSampleL = SHRT_MAX;
internal int16 maxSampleR = SHRT_MAX;

#include "wave.h"

#include "handleevents.h"

inline int minimum(int x, int y)
{
	return y ^ ((x ^ y) & -(x < y));
}

inline int maximum(int x, int y)
{
	return x ^ ((x ^ y) & -(x < y));
}

void AudioPostMix(void *udata, uint8 *stream, int len)
{
	float64 suml = 0.0;
	float64 sumr = 0.0;
	float64 maxl = FLT_MIN;
	float64 maxr = FLT_MIN;
	for(int i = 0; i < len / 4; ++i)
	{
		int16 l = abs((stream[i] << 8) | (stream[++i]));
		int16 r = abs((stream[++i] << 8) | (stream[++i]));
		float32 la = l / 32767.0f;
		float32 ra = r / 32767.0f;
		suml += (la * la);
		sumr += (ra * ra);
		maxl = maximum(l, maxl);
		maxr = maximum(r, maxr);
	}
	maxl = (maxl * maxl) / 32768.0f;
	maxr = (maxr * maxr) / 32768.0f;

	float32 rmsal = sqrt(suml / (len / 4));
	float32 rmsar = sqrt(sumr / (len / 4));
	float32 dbal = 20 * log10(rmsal);
	float32 dbar = 20 * log10(rmsar);
	float32 rmspl = sqrt(maxl / 32768.0f);
	float32 rmspr = sqrt(maxr / 32768.0f);
	float32 dbpl = 20 * log10(rmspl);
	float32 dbpr = 20 * log10(rmspr);
	vuAvgLeft = exp(dbal * (log(10.0f) / 20.0f));
	vuAvgRight = exp(dbar * (log(10.0f) / 20.0f));
	vuPeakLeft = exp(dbpl * (log(10.0f) / 20.0f));
	vuPeakRight = exp(dbpr * (log(10.0f) / 20.0f));
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
	// rendererFlags |= SDL_RENDERER_PRESENTVSYNC;
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

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
	do
	{
		HandleEvents(renderer, window, &mouse, music, &cursor);
		SDL_SetRenderDrawColor(renderer, 31, 31, 31, 255);
		SDL_RenderClear(renderer);

		SDL_RenderCopy(renderer, wavTexture, NULL, &wavRect);

		boxColor(renderer, Window_Width - 60, 0, Window_Width - 2, Window_Height, 0xFF000000);

		boxColor(renderer, Window_Width - 58, Window_Height, Window_Width - 38, 
		         (Window_Height) - (vuPeakLeft * (Window_Height - 100)), COLOR_VUPEAK);
		boxColor(renderer, Window_Width - 28, Window_Height, Window_Width - 8,
		         (Window_Height) - (vuPeakRight * (Window_Height - 100)), COLOR_VUPEAK);

		boxColor(renderer, Window_Width - 58, Window_Height, Window_Width - 38, 
		         (Window_Height) - (vuAvgLeft * (Window_Height - 100)), COLOR_VUAVERAGE);
		boxColor(renderer, Window_Width - 28, Window_Height, Window_Width - 8, 
		         Window_Height - (vuAvgRight * (Window_Height - 100)), COLOR_VUAVERAGE);

		hlineColor(renderer, Window_Width - 60, Window_Width - 2, 100, COLOR_LIGHTLINE);

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