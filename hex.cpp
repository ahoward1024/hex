#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <float.h>


#include "SDL/SDL.h"
#include "SDL/SDL_framerate.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_mixer.h"


#include "xtrace.h"
#include "datatypes.h"
#define  COLORMODE_ARGB8888
#include "colors.h"
#include "timer.h"
#include "drawing.h"

#include "resource.h"

global bool Global_running = true;
global bool Global_paused = true;
global int Window_Width  = 1280;
global int Window_Height = 720;
global int Half_WW = Window_Width / 2;
global int Fourth_WW = Window_Width / 4;
global int Half_WH = Window_Height / 2;
global int Fourth_WH = Window_Height / 4;
global int BORDER = 10;
global bool FLIP = false;
global bool UP = false;
global int LENGTH = 9;
const global int INCREMENT = 8;
global bool CLEAR = true;
global int REFRATE = 1;
global int VOLUME = SDL_MIX_MAXVOLUME;
global bool DELAY = false;
global float32 cursor = 10.0f;

static const uint32 COLOR_BACKGROUNDC  = 0xFF1F1F1F;
static const uint32 COLOR_LEFTCHANNEL  = 0xFF80FF80; 
static const uint32 COLOR_RIGHTCHANNEL = 0xFFFF8080;
static const uint32 COLOR_LIGHTLINE    = 0xFF4A4A4A;

#include "wave.h"

#include "handleevents.h"

void AudioPostMix(void *udata, uint8 *stream, int len)
{
	SDL_Surface *s = (SDL_Surface *)udata;
	int32 *audio = (int32 *)calloc(len / 4, sizeof(int32));
	SDL_FillRect(s, NULL, 0xFF000000);
	memcpy(audio, stream, len);
	int16 lmax = SHRT_MIN;
	int16 rmax = SHRT_MIN;
	for(int i = 0; i < len / 2; ++i)
	{
		if(audio[i])
		{
			int32 s = audio[i];
			// Flip the left and right channels because of endianness.
			int16 l = s & 0xFFFF;
			int16 r = s >> 16;
			lmax = l ^ ((l ^ lmax) & -(l < lmax));
			rmax = r ^ ((r ^ rmax) & -(r < rmax));
		}
		else
		{
			break;
		}
	}

	int ly = lmax * s->h / 0x7FFF;
	int ry = rmax * s->h / 0x7FFF;
	SDL_Rect lr = { 0, s->h - ly, 10, ly };
	SDL_Rect rr = { 12, s->h - ry, 10, ry };
	SDL_FillRect(s, &lr, 0xFF00FF00);
	SDL_FillRect(s, &rr, 0xFF00FF00);

	free(audio);
}

int main(int argc, char **argv)
{
	printf("Hello world\n\n");

	srand((unsigned)time(NULL));

	if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0)
	{
		printf("Error initializing SDL. Exiting.\n");
		printf("%s\n", SDL_GetError());
		return -1;
	}
	atexit(SDL_Quit);

	Mix_Init(0);

	if(Mix_OpenAudio(48000, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		printf("Failed to open SDL_Mixer: %s\nExiting.\n", Mix_GetError());
		return -1;
	}

	const char *musicFileName = "../res/test.wav";
	WAVFile wavFile = openWAVFile(musicFileName);
	printWAVFile(wavFile);
	SDL_Surface *wavSurface = createWaveformSurface(wavFile, Window_Height - 20);
	closeWAVFile(wavFile);

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
	SDL_Surface *peaks = SDL_CreateRGBSurface(0, 22, Window_Height, 32,
	                                          0x00FF0000,
	                                          0x0000FF00,
	                                          0x000000FF,
	                                          0xFF000000);
	SDL_SetSurfaceBlendMode(peaks, SDL_BLENDMODE_BLEND);
	Mix_SetPostMix(AudioPostMix, peaks);

	uint32 windowFlags = 0;
	// windowFlags |= SDL_WINDOW_FULLSCREEN;
	SDL_Window *window = SDL_CreateWindow("Hex: 0", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
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
	SDL_Surface *surface = SDL_GetWindowSurface(window);
	SDL_Renderer *renderer = SDL_CreateSoftwareRenderer(surface);

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
	TTF_Init();
	if(!TTF_WasInit())
	{
		printf("Could not init TTF. Exiting.\n");
		printf("%s\n", TTF_GetError());
		return -1;
	}
	TTF_Font *fontConsolas24 = TTF_OpenFont("../res/consolas.ttf", 24);

	Mix_VolumeMusic(VOLUME);
	#if 1
	if(Mix_PlayMusic(music, -1) == -1)
	{
		printf("Failed to play music.\n");
		return -1;
	}
	#endif
	Mix_PauseMusic();
	
	uint64 startClock = SDL_GetTicks();
	int targetMSPF = 16;
	int targetFPS = 60;
	Mouse mouse;
	SDL_Rect wsRect = { 10, Half_WH - (wavSurface->h / 2), surface->w - 50, wavSurface->h };
	SDL_Rect wsRectSource = { 0, 0, wavSurface->w, wavSurface->h };
	SDL_RenderSetClipRect(renderer, NULL);
	do
	{
		HandleEvents(renderer, window, &mouse, music, &cursor, surface, wavSurface, &wsRect);

		clear(renderer, 0xFF1F1F1F);

		SDL_BlitScaled(wavSurface, &wsRectSource, surface, &wsRect);
		SDL_Rect peaksRect = { Window_Width - peaks->w - 5, 0, peaks->w, peaks->h };
		SDL_BlitSurface(peaks, NULL, surface, &peaksRect);
		vline(surface, 10, Window_Height - 10, cursor, 0xFFFFFF00);

		// TODO(alex): Make the cursor move at the actual rate of the song.
		if(!Mix_PausedMusic() && Mix_PlayingMusic()) 
			cursor += (wavFile.duration / (float32)wavSurface->w);
		if(cursor >= (surface->w - 50)) cursor = 10.0f;
		SDL_UpdateWindowSurface(window);

		uint64 ms = SDL_GetTicks() - startClock;
		// Delay the screen refresh so we are at a constant frame rate.
		while(ms < targetMSPF) { ms = SDL_GetTicks() - startClock; }
		uint64 fps = 0;
		if(ms > 0) fps = (1.0f/(float32)ms) * 1000.0f;
		if(fps < targetFPS)
		{
			time_t rawtime;
			struct tm *timeinfo;
			time(&rawtime);
			timeinfo = localtime(&rawtime);
			printf("Below %d fps:\n%s%.4f Seconds in\n\n", targetFPS, asctime(timeinfo),
			       (float)SDL_GetTicks() / 1000.0f);
		}
		char wtextBuffer[128];
		sprintf(wtextBuffer, "Hex: ms/f: %llu, fps: %llu, %s, %s", ms, fps, 
		        (CLEAR ? "CLEAR" : "NOCLEAR"), getBlendMode(wavSurface));
		SDL_SetWindowTitle(window, wtextBuffer);
		startClock = SDL_GetTicks();
	} while(Global_running);

	IMG_Quit();
	TTF_CloseFont(fontConsolas24);
	Mix_CloseAudio();
	SDL_FreeSurface(surface);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	printf("Goodbye.\n");

	return 0;
}