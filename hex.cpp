#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#include "SDL/SDL.h"
#include "SDL/SDL_framerate.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_mixer.h"

#include "drawing.h"
#include "hexagon.h"

#include "datatypes.h"
#include "xtrace.h"
#include "colors.h"
#include "timer.h"

#include "resource.h"

global bool Global_running = true;
global bool Global_paused = true;
global int Window_Width  = 1920;
global int Window_Height = 1080;
global bool FLIP = false;
global bool UP = false;
global int LENGTH = 9;
const global int INCREMENT = 8;
global bool CLEAR = true;
global int REFRATE = 1;
global int VOLUME = SDL_MIX_MAXVOLUME;
global bool DELAY = false;

struct Rect
{
	SDL_Rect rect;
	int32 vx;
	int32 vy;
	uint32 color;
};

struct RectList
{
	Rect *rects;
	int   size;
};

struct HexList
{
	Hexagon *hexes;
	int      size;
};

void PulsingHexes(SDL_Renderer *renderer, SDL_Surface *surface, HexList *hlist)
{
	uint32 color = 0xFFFFFFFF;
	uint8 r = 0xFF;
	uint8 g = 0xFF;
	uint8 b = 0xFF;
	const uint8 a = 0xFF;
	if(!Global_paused)
	{
		r += (rand() % 255);
		g += (rand() % 255);
		b += (rand() % 255);

		if(LENGTH == 9) FLIP = true;
		else if(LENGTH == 1705) FLIP = true;

		if(FLIP)
		{
			FLIP = false;
			UP = !UP;
			SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
			SDL_RenderClear(renderer);
		}

		if(UP) LENGTH += INCREMENT;
		if(!UP) LENGTH -= INCREMENT;
	}

	for(int j = 0; j < 9; ++j)
	{
		for(int i = 0; i < 7; ++i)
		{
			uint32 color = (b << 0) | (g << 8) | (r << 16) | (a << 24);
			DrawHexToSurface(surface, hlist->hexes[j]);
		}
	}
}

Rect createRect()
{
	Rect rect = {0};
	SDL_Rect sdlrect = {0};
	sdlrect.w = 10;
	sdlrect.h = 10;
	sdlrect.x = rand() % Window_Width;
	sdlrect.y = rand() % Window_Height;	

	rect.rect = sdlrect;

	int ra = rand() & 1;
	if(ra) rect.vx = 1;
	else rect.vx = -1;
	ra = rand() & 1;
	if(ra) rect.vy = 1;
	else rect.vy = -1;

	uint8 r = (rand() % 256) + 1;
	uint8 g = (rand() % 256) + 1;
	uint8 b = (rand() % 256) + 1;
	uint8 a = (rand() % 256) + 128;

	rect.color = (b << 0) | (g << 8) | (r << 16) | (a << 24);
	// printf("%d\n", a);

	return rect;
}

RectList createRectList(int size)
{
	RectList rectList = {0};
	rectList.size = size;
	rectList.rects = (Rect *)malloc(rectList.size * sizeof(Rect));
	for(int i = 0; i < rectList.size; ++i)
	{
		rectList.rects[i] = createRect();
	}
	return rectList;
}

SDL_Point * createHexGridPoints()
{
	int w2 = Window_Width / 2;
	int w4 = Window_Width / 4;
	int h2 = Window_Height / 2;
	int h4 = Window_Height / 4;

	static SDL_Point plist[9];
	plist[0] = { w4, h4 };
	plist[1] = { w4, h2 };
	plist[2] = { w4, h2 + h4 };
	plist[3] = { w2, h4 };
	plist[4] = { w2, h2 };
	plist[5] = { w2, h2 + h4 };
	plist[6] = { w2 + w4, h4 };
	plist[7] = { w2 + w4, h2 };
	plist[8] = { w2 + w4, h2 + h4 };

	return plist;
}

void checkRect(Rect *rect)
{
	if(rect->rect.x <= 0)
	{
		rect->vx *= -1;
	} 
	else if(rect->rect.x >= (Window_Width - rect->rect.w))
	{
		rect->vx *= -1;
	}

	if(rect->rect.y <= 0)
	{
		rect->vy *= -1;
	}
	else if(rect->rect.y >= (Window_Height - rect->rect.h))
	{
		rect->vy *= -1;
	}
}

void moveRect(Rect *rect)
{
	rect->rect.x += rect->vx;
	rect->rect.y += rect->vy;
}

void BouncingSquares(SDL_Surface *surface, RectList rectList)
{
	for(int i = 0; i < rectList.size; ++i)
	{
		checkRect(&rectList.rects[i]);
		SDL_FillRect(surface, &rectList.rects[i].rect, rectList.rects[i].color);
		moveRect(&rectList.rects[i]);
	}
}

int colorClamp(int value)
{
	if(value > 255) return 255;
	if(value < 0) return 0;
	else return value;
}

// >>> Adapted from: http://jcatki.no-ip.org:8080/SDL_mixer/demos/sdlwav.c
/* set this to any of 512,1024,2048,4096              */
/* the lower it is, the more FPS shown and CPU needed */
#define BUFFER 1024
#define W 1920 /* NEVER make this be less than BUFFER! */
#define H 1080
#define H2 (H/2)
#define H4 (H/4)
#define Y(sample) (((sample)*H)/4/0x7FFF)

Sint16 stream[2][BUFFER*2*2];
int len = BUFFER * 2 * 2, done = 0, bits = 0, which = 0, sample_size = 0, position = 0, rate = 0;
float dy;

static void postmix(void *udata, Uint8 *_stream, int _len)
{
	SDL_Surface *s = (SDL_Surface *)udata;
	position += _len / sample_size;
	/* fprintf(stderr,"pos=%7.2f seconds \r",position/(float)rate); */
	/* save the stream buffer and indicate that we need a redraw */
	len = _len;
	memcpy(stream[(which + 1) % 2], _stream, len > (s->w * 4) ? (s->w * 4) : len);
	which = (which + 1) % 2;
}

void posNegWaveform(SDL_Surface *s, uint8 alpha)
{
	int x;
	Sint16 *buf;

	/*fprintf(stderr,"len=%d   \r",len); */

	buf=stream[which];
	
	SDL_LockSurface(s);

	/* draw the wav from the saved stream buffer */
	SDL_Rect r;
	SDL_FillRect(s, NULL, 0x00000000);
	for(x = 0; x < (W * 2); x++)
	{
		const int X = x >> 1, b = x & 1, t = H4 + H2 * b;
		int y1,h1;
		int top = 10;
		if(buf[x] < 0)
		{
			h1 = -Y(buf[x]);
			y1 = t - h1;

			#if 1
			if(h1 > 0)
			{
				r = { X, y1 - top, 1, top };
				SDL_FillRect(s, &r, 0xDB00FFFF);
			}
			#endif
		}
		else
		{
			y1 = t;
			h1 = Y(buf[x]);

			#if 1
			if(h1 > 0)
			{
				r = { X, y1 + h1, 1, top };
				SDL_FillRect(s, &r, 0xDB00FFFF);
			}
			#endif
		}
		#if 0
		{
			r = { X, H2 * b, 1 };
			r.h = y1 - r.y;
			uint8 red = colorClamp(r.h >> 2);
			uint8 green = colorClamp(0x80);
			uint8 blue = colorClamp(r.h >> 2);
			uint32 color = (alpha << 24) | (red << 16) | (green << 8) | (blue << 0);
			SDL_FillRect(s, &r, color);
		}
		#endif
		{
			
			#if 1
			r = { X, y1, 1, h1 };
			uint8 red = colorClamp(r.h * 2);
			uint8 green = colorClamp((r.h * 4) % 2);
			uint8 blue = colorClamp(r.h << 4);
			uint32 color = (0x80 << 24) | (red << 16) | (green << 8) | (blue << 0);
			//color = 0xDBFF00FFF;
			SDL_FillRect(s, &r, color);
			#endif
		}
		#if 0
		{
			r = { X, y1+h1, 1 };
			r.h = H2 + H2 * b - r.y;
			uint8 red = colorClamp(0x80);
			uint8 green = colorClamp(r.h >> 2);
			uint8 blue = colorClamp(r.h >> 2);
			uint32 color = (alpha << 24) | (red << 16) | (green << 8) | (blue << 0);
			SDL_FillRect(s, &r, color);
		}
		#endif
	}
	SDL_UnlockSurface(s);
}
// <<< Adapted from: http://jcatki.no-ip.org:8080/SDL_mixer/demos/sdlwav.c

const char *getBlendMode(SDL_Surface *surface)
{
	SDL_BlendMode bm;
	SDL_GetSurfaceBlendMode(surface, &bm);
	if(bm == SDL_BLENDMODE_NONE) return "NONE";
	else if(bm == SDL_BLENDMODE_BLEND) return "BLEND";
	else if(bm == SDL_BLENDMODE_ADD) return "ADD";
	else if(bm == SDL_BLENDMODE_MOD) return "MOD";
	else return "???";
}

void HandleEvents(SDL_Renderer *renderer, SDL_Window *window, 
                  SDL_Surface *wavSurface, uint32 *color1, uint32 *color2, HexList *hexList, 
                  SDL_Point mid, Timer *timer, Mix_Music *music)
{
	SDL_Event event;
	if(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
			case SDL_QUIT:
			{
				Global_running = false;
			} break;
			case SDL_MOUSEWHEEL:
			{
				switch(event.wheel.type)
				{
					case SDL_MOUSEWHEEL:
					{
						int amount;
						if(event.wheel.y > 0) amount = 16;
						else amount = -16;
						if(VOLUME < SDL_MIX_MAXVOLUME)
						{
							VOLUME += amount;
						}
						else if(VOLUME > 0)
						{
							VOLUME += amount;
						}
						Mix_VolumeMusic(VOLUME);
					} break;
				}
			}
			case SDL_KEYDOWN:
			{
				SDL_Keycode key = event.key.keysym.sym;
				switch(key)
				{
					case SDLK_ESCAPE:
					{
						Global_running = false;
					} break;
					case SDLK_EQUALS:
					{
						if(LENGTH <= 9999) LENGTH += INCREMENT;
					} break;
					case SDLK_MINUS:
					{
						if(LENGTH > 9 + INCREMENT) LENGTH -= INCREMENT;
						else LENGTH = 9;
					} break;
					case SDLK_SPACE:
					{
						Global_paused = !Global_paused;
						if(Mix_PausedMusic()) Mix_ResumeMusic();
						else Mix_PauseMusic();
					} break;
					case SDLK_c:
					{
						CLEAR = !CLEAR;
					} break;
					case SDLK_v:
					{
						SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
						SDL_RenderClear(renderer);
					}
					case SDLK_m:
					{
						*color1 = createRandomColor();
						*color2 = createRandomColor();
					} break;
					case SDLK_g:
					{
						hexList->hexes[0].m = mid;
						SDL_Point start = mid;
						for(int i = 1; i < hexList->size; ++i)
						{
							int nextHexSide = rand() % 7;
							hexList->hexes[i].m = NextHexFromMid(hexList->hexes[0], (Hex_Side)nextHexSide);
							start = hexList->hexes[i].m;
						}
					} break;
					case SDLK_b:
					{
						SDL_BlendMode bm;
						SDL_GetSurfaceBlendMode(wavSurface, &bm);
						if(bm == SDL_BLENDMODE_BLEND) SDL_SetSurfaceBlendMode(wavSurface, SDL_BLENDMODE_MOD);
						else if(bm == SDL_BLENDMODE_NONE) SDL_SetSurfaceBlendMode(wavSurface, SDL_BLENDMODE_MOD);
						else SDL_SetSurfaceBlendMode(wavSurface, SDL_BLENDMODE_BLEND);
					} break;
					case SDLK_n:
					{
						SDL_SetSurfaceBlendMode(wavSurface, SDL_BLENDMODE_NONE);
					} break;
					case SDLK_t:
					{
						if(timer->_bound == 1) timer->_bound = 500;
						else timer->_bound = 1;
						DELAY = !DELAY;
					} break;
					case SDLK_r:
					{
						Mix_RewindMusic(); // Does not work with WAV files...
					}
					case SDLK_RIGHT:
					{
					} break;
					case SDLK_LEFT:
					{
					} break;
				}
			}
			case SDL_WINDOWEVENT:
			{
				switch(event.window.event)
				{
					case SDL_WINDOWEVENT_MINIMIZED:
					{
						Global_paused = true;
					} break;
				}
			} break;
		}
	}
}

/**
 * Convert seconds into hh:mm:ss format
 * Params:
 *  seconds - seconds value
 * Returns: hms - formatted string
 **/
 char *seconds_to_time(float64 raw_seconds) {
  char *hms;
  int hours, hours_residue, minutes, seconds, milliseconds;
  hms = (char *)malloc(100);
 
  sprintf(hms, "%f", raw_seconds);
 
  hours = (int) raw_seconds/3600;
  hours_residue = (int) raw_seconds % 3600;
  minutes = hours_residue/60;
  seconds = hours_residue % 60;
  milliseconds = 0;
 
  // get the decimal part of raw_seconds to get milliseconds
  char *pos;
  pos = strchr(hms, '.');
  int ipos = (int) (pos - hms);
  char decimalpart[15];
  memset(decimalpart, ' ', sizeof(decimalpart));
  strncpy(decimalpart, &hms[ipos+1], 3);
  milliseconds = atoi(decimalpart);
   
  sprintf(hms, "%d:%d:%d.%d", hours, minutes, seconds, milliseconds);
  return hms;
}


// http://truelogic.org/wordpress/2015/09/04/parsing-a-wav-file-in-c/
struct WAVFile
{
	FILE   *file;
	char   *filename;
	// HEADER
	uint8   riff[4];
	uint32  overallSize;
	uint8   wave[4];
	uint8   fmtChunk[4];
	uint32  fmtLength;
	uint32  fmtType;
	uint32  channels;
	uint32  sampleRate;
	uint32  byteRate;
	uint32  blockAlign;
	uint32  bitsPerSample;
	uint8   data[4];
	uint32  dataSize;
	// END HEADER
	uint64  numSamples;
	uint64  sampleSize;
	float64 duration;
};

const char *getWAVFormatType(uint32 fmtType)
{
	switch(fmtType)
	{
		case 1: return "PCM"; break;
		case 6: return "A-law"; break;
		case 7: return "Mu-law"; break;
		default: return "Unknown";
	}
}

void printWAVFile(WAVFile wav)
{
	printf("WAVFile: %s\n", wav.filename);
	printf("\t%c%c%c%c\n", wav.riff[0], wav.riff[1], wav.riff[2], wav.riff[3]);
	printf("\tSize in bytes: %d\n", wav.overallSize);
	printf("\tFormat: %c%c%c%c%c%c%c%c\n", wav.wave[0], wav.wave[1], wav.wave[2], wav.wave[3],
	       wav.fmtChunk[0], wav.fmtChunk[1], wav.fmtChunk[2], wav.fmtChunk[3]);
	printf("\tFormat length: %d\n", wav.fmtLength);
	printf("\tFormat type: %s\n", getWAVFormatType(wav.fmtType));
	printf("\tChannels: %d\n", wav.channels);
	printf("\tSample rate: %d\n", wav.sampleRate);
	printf("\tByte rate: %d\n", wav.byteRate);
	printf("\tBlock align: %d\n", wav.blockAlign);
	printf("\tBits per sample: %d\n", wav.bitsPerSample);
	printf("\tData: %c%c%c%c\n", wav.data[0], wav.data[1], wav.data[2], wav.data[3]);
	printf("\tData size: %d\n", wav.dataSize);
	printf("\tNumber of samples: %d\n", wav.numSamples);
	printf("\tSample size: %d\n", wav.sampleSize);
	printf("\tDuration in seconds: %.4f\n", wav.duration);
	printf("\tDuration in time: %s\n", seconds_to_time(wav.duration));
}

inline uint32 littleToBig2(uint8 buffer[2])
{
	return buffer[0] | (buffer[1] << 8);
}

inline uint32 littleToBig4(uint8 buffer[4])
{
	return buffer[0] | (buffer[1] << 8) | (buffer[2] << 16) | (buffer[3] << 24);
}

WAVFile openWAVFile(const char *filename)
{
	WAVFile wav = {0};
	wav.file = fopen(filename, "rb");
	if(wav.file)
	{
		uint8 buffer4[4];
		uint8 buffer2[2];
		wav.filename = (char *)malloc(strlen(filename) * sizeof(char));
		strcpy(wav.filename, filename);
		fread(wav.riff, 4, 1, wav.file);
		fread(buffer4, 4, 1, wav.file);
		wav.overallSize = littleToBig4(buffer4);
		fread(wav.wave, 4, 1,  wav.file);
		fread(wav.fmtChunk, 4, 1, wav.file);
		fread(buffer4, 4, 1, wav.file);
		wav.fmtLength = littleToBig4(buffer4);
		fread(buffer2, 2, 1, wav.file);
		wav.fmtType = littleToBig2(buffer2);
		fread(buffer2, 2, 1, wav.file);
		wav.channels = littleToBig2(buffer2);
		fread(buffer4, 4, 1, wav.file);
		wav.sampleRate = littleToBig4(buffer4);
		fread(buffer4, 4, 1, wav.file);
		wav.byteRate = littleToBig4(buffer4);
		fread(buffer2, 2, 1, wav.file);
		wav.blockAlign = littleToBig2(buffer2);
		fread(buffer2, 2, 1, wav.file);
		wav.bitsPerSample = littleToBig2(buffer2);
		fread(wav.data, 4, 1, wav.file);
		fread(buffer4, 4, 1, wav.file);
		wav.dataSize = littleToBig4(buffer4);

		wav.numSamples = (8 * wav.dataSize) / (wav.channels * wav.bitsPerSample);
		wav.sampleSize = (wav.channels * wav.bitsPerSample) /  8;

		wav.duration = (float64)wav.overallSize / wav.byteRate;
	}
	else
	{
		printf("Could not open file %s \n", filename);
	}

	return wav;
}

void closeWAVFile(WAVFile wav)
{
	fclose(wav.file);
	free(wav.filename);
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

	const char *musicFileName = "../res/res.wav";
	WAVFile wavFile = openWAVFile(musicFileName);
	printWAVFile(wavFile);

	Mix_Music *music = Mix_LoadMUS(musicFileName);
	if(!music)
	{
		printf("Could not load music.\n%s\nExiting.\n", Mix_GetError());
		return -1;
	}

	int audio_channels = 0, audio_rate = 0;
	Uint16 audio_format;
	Mix_QuerySpec(&audio_rate, &audio_format, &audio_channels);
	int bits = audio_format & 0xFF;
	sample_size = bits / 8 + audio_channels;
	rate = audio_rate;

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

	SDL_Surface *wavSurface = SDL_CreateRGBSurface(0, 1920, 1080, 32, 
	                                              0x00FF0000,
	                                              0x0000FF00,
	                                              0x000000FF,
	                                              0xFF000000);
	SDL_SetSurfaceBlendMode(wavSurface, SDL_BLENDMODE_MOD);

	if(!renderer)
	{
		printf("Could not create renderer. Exiting.\n");
		printf("%s\n", SDL_GetError());
		return -1;
	}
	
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

	Mix_SetPostMix(postmix, wavSurface);

	TTF_Init();
	if(!TTF_WasInit())
	{
		printf("Could not init TTF. Exiting.\n");
		printf("%s\n", TTF_GetError());
		return -1;
	}

	TTF_Font *fontConsolas24 = TTF_OpenFont("../res/consolas.ttf", 24);

	SDL_Color white = { 255, 255, 255, 255 };

	SDL_Point *plist = createHexGridPoints();

	RectList rlist = createRectList(100);

	SDL_Point mid = { Window_Width / 2, Window_Height / 2 };

	int hexListIndex = 0;
	SDL_Point p = { 0, 0 };
	int rows = 120;
	int cols = 108;
	HexList hexList;
	hexList.size = rows * (cols * 2);
	hexList.hexes = (Hexagon *)calloc(hexList.size, sizeof(Hexagon));
	for(int i = 0; i < rows; ++i)
	{
		for(int j = 0; j < cols; ++j)
		{
			Hexagon hex = { p, LENGTH, 0xFFFFFFFF };
			int tmp = hexListIndex;
			hexList.hexes[hexListIndex++] = hex;
			p = { NextHexFromMid(hexList.hexes[tmp], HEXSIDE_UR) };
			hex = { p, LENGTH, 0xFFFFFFFF };
			hexList.hexes[hexListIndex++] = hex;
			p = { NextHexFromMid(hexList.hexes[tmp], HEXSIDE_MR) };
		}
		p = { NextHexFromMid(hexList.hexes[i * (cols * 2)], HEXSIDE_D) };
	}
	
	uint32 color1 = 0xFFFF0000;
	uint32 color2 = 0xFF00FF00;

	SDL_Rect originalWindowSize = { Window_Width, Window_Height };

	SDL_Rect scaledWindowSize = { 3456, 1944 };

	Timer ref = CreateTimer(1);
	Timer gen = CreateTimer(1000);

	Mix_VolumeMusic(VOLUME);

	if(Mix_FadeInMusic(music, 1, 3000) == -1)
	{
		printf("Failed to play music.\n");
		return -1;
	}
	Mix_PauseMusic();

	uint64 startClock = SDL_GetTicks();
	do
	{
		HandleEvents(renderer, window, wavSurface, &color1, &color2, &hexList, mid, &ref, music);
		#if 1
		if(CLEAR) clear(renderer);
		#endif

		// if(TickTimer(&ref)) refresh(wavSurface);

		#if 0
		if(TickTimer(&gen))
		{
			Point start = mid;
			for(int i = 1; i < hexAmount; ++i)
			{
				int nextHexSide = rand() % 7;
				hexList[i] = NextHexFromMid(start, 9, (Hex_Side)nextHexSide);
				start = hexList[i];
			}
		}
		#endif

		#if 1
		// Draw random hex grid
		for (int i = 0; i < hexList.size; ++i)
		{
			if(i == 7 || i == 8)
			{
				DrawHexToSurface(surface, hexList.hexes[i]);
			}
			else
			{
				DrawHexToSurface(surface, hexList.hexes[i]);
			}
		}
		#endif
		if(DELAY)
		{
			if(TickTimer(&gen)) posNegWaveform(wavSurface, 0xFF);
		}
		else
		{
			posNegWaveform(wavSurface, 0xFF);
		}

		// BouncingSquares(surface, rlist);
		// PulsingHexes(renderer, surface, plist);

		SDL_BlitSurface(wavSurface, NULL, surface, NULL);
		SDL_UpdateWindowSurface(window);

		uint64 ms = SDL_GetTicks() - startClock;
		// Delay the screen refresh so we are at a constant frame rate.
		while(ms < 33) { ms = SDL_GetTicks() - startClock; }
		uint64 fps = 0;
		if(ms > 0) fps = (1.0f/(float32)ms) * 1000.0f;
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