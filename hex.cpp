#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "SDL/SDL.h"
#include "SDL/SDL_framerate.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_mixer.h"

#include "datatypes.h"
#include "xtrace.h"
#include "colors.h"
#include "timer.h"

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

struct Point
{
	uint32 x;
	uint32 y;
};

enum Hex_Side
{
	HEXSIDE_M  = 0,
	HEXSIDE_U  = 1,
	HEXSIDE_UR = 2,
	HEXSIDE_DR = 3,
	HEXSIDE_D  = 4,
	HEXSIDE_DL = 5,
	HEXSIDE_UL = 6
};

struct Rect
{
	SDL_Rect rect;
	int32 vx;
	int32 vy;
	uint32 color;
};

struct Hex
{
	Point  point;
	uint32 color;
};

struct RectList
{
	Rect *rects;
	int   size;
};

void setPixel(SDL_Surface *surface, const int32 x, const int32 y, const uint32 color)
{
	if(x > 0 && y > 0 && x < surface->w  && y < surface->h)
	{
		uint32 *pixels = (uint32 *)surface->pixels;
		pixels[(y * surface->w) + x] = color;
	}
}

void DrawTextToSurface(SDL_Surface *dest, int x, int y, const char *text, 
                       TTF_Font *font, SDL_Color color)
{
	SDL_Surface *surface;
	if(surface = TTF_RenderText_Blended(font, text, color))
	{
		SDL_Rect rect;
		rect.x = x;
		rect.y = y;
		rect.w = surface->w;
		rect.h = surface->h;
		SDL_BlitSurface(surface, NULL, dest, &rect);
		SDL_FreeSurface(surface);
	}
	else
	{
		printf("TTF error.\n%s\n", TTF_GetError());
	}
}

Point NextHexFromMid(const Point start, uint32 length, Hex_Side side)
{
	if(side == HEXSIDE_M) return start;
	Point p = {};
	uint32 l = length-1;
	uint32 h = l / 2;
	uint32 f = h / 2;
	uint32 d = 2 * l;
	switch(side)
	{
		case HEXSIDE_U:  p = { start.x, start.y-d }; break;
		case HEXSIDE_UR: p = { start.x+l+h+f, start.y-l }; break;
		case HEXSIDE_DR: p = { start.x+l+h+f, start.y+l }; break;
		case HEXSIDE_D:  p = { start.x, start.y+d }; break;
		case HEXSIDE_DL: p = { start.x-l-h-f, start.y-l }; break;
		case HEXSIDE_UL: p = { start.x-l-h-f, start.y+l }; break;
	}

	return p;
}

void line(SDL_Surface *surface, const uint32 x1, const uint32 y1, const uint32 x2, const uint32 y2, 
          const uint32 color)
{
	int32 x = (int32)x1;
	int32 y = (int32)y1;
	int32 w = x2 - x;
	int32 h = y2 - y;
	int32 dx1 = 0, dy1 = 0, dx2 = 0, dy2 = 0;
	if (w < 0) dx1 = -1 ; else if (w > 0) dx1 = 1;
	if (h < 0) dy1 = -1 ; else if (h > 0) dy1 = 1;
	if (w < 0) dx2 = -1 ; else if (w > 0) dx2 = 1;
	int32 longest = abs(w) ;
	int32 shortest = abs(h) ;
	if (!(longest > shortest)) {
		longest = abs(h);
		shortest = abs(w);
		if (h < 0) dy2 = -1; 
		else if (h > 0) dy2 = 1;
		dx2 = 0;
	}
	int numerator = longest >> 1 ;
	for (int i = 0; i <= longest; ++i) {
		setPixel(surface, x, y,color);
		numerator += shortest;
		if (!(numerator < longest)) {
			numerator -= longest;
			x += dx1;
			y += dy1;
		} else {
			x += dx2;
			y += dy2;
		}
	}
}

void vline(SDL_Surface *surface, const int32 y1, const int32 y2, const uint32 x, 
           const uint32 color)
{
	for(int32 i = y1; i <= y2; ++i)
		setPixel(surface, x, i, color);
}

void hline(SDL_Surface *surface, const int32 x1, const int32 x2, const uint32 y, 
           const uint32 color)
{
	for(int32 i = x1; i <= x2; ++i)
		setPixel(surface, i, y, color);
}

void DrawHexToSurface(SDL_Surface *surface, const Point p, const uint32 size, const uint32 color)
{
	SDL_LockSurface(surface);

	int32 x = p.x;
	int32 y = p.y;

	uint32 l = size-1;
	uint32 h = l / 2;
	uint32 f = h / 2;

	hline(surface, x-h, x+h, y-l, color);
	hline(surface, x-h, x+h, y+l, color);

	line(surface, x+h, y-l, x+l+f, y, color);
	line(surface, x+l+f, y, x+h, y+l, color);
	line(surface, x-h, y+l, x-l-f, y, color);
	line(surface, x-l-f, y, x-h, y-l, color);

	SDL_UnlockSurface(surface);
}

void DrawHexToSurface(SDL_Surface *surface, const int32 x, const int32 y, const uint32 length, 
                      const uint32 color)
{
	SDL_LockSurface(surface);

	uint32 l = length-1;
	uint32 h = l / 2;
	uint32 f = h / 2;

	for(int i = x-h; i <= x+h; ++i)
	{
		setPixel(surface, i, y-l, color);
		setPixel(surface, i, y+l, color);
	}

	line(surface, x+h, y-l, x+l+f, y, color);
	line(surface, x+l+f, y, x+h, y+l, color);
	line(surface, x-h, y+l, x-l-f, y, color);
	line(surface, x-l-f, y, x-h, y-l, color);

	SDL_UnlockSurface(surface);
}

uint32 createRandomColor()
{
	uint32 color;
	uint8 r = (rand() % 256) + 1;
	uint8 g = (rand() % 256) + 1;
	uint8 b = (rand() % 256) + 1;
	uint8 a = (rand() % 256) + 1;

	color = (b << 0) | (g << 8) | (r << 16) | (a << 24);
	return color;
}

void PulsingHexes(SDL_Renderer *renderer, SDL_Surface *surface, Point *plist)
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
		Point p = plist[j];
		for(int i = 0; i < 7; ++i)
		{
			uint32 color = (b << 0) | (g << 8) | (r << 16) | (a << 24);
			DrawHexToSurface(surface, NextHexFromMid(p, LENGTH, (Hex_Side)i), LENGTH, color);
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

Point * createHexGridPoints()
{
	uint32 w2 = Window_Width / 2;
	uint32 w4 = Window_Width / 4;
	uint32 h2 = Window_Height / 2;
	uint32 h4 = Window_Height / 4;

	static Point plist[9];
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

void HandleEvents(SDL_Renderer *renderer, SDL_Window *window, uint32 *color1, uint32 *color2,
                  Point *hexList, int hexAmount, Point mid, Timer *timer)
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
						Point start = mid;
						for(int i = 1; i < hexAmount; ++i)
						{
							int nextHexSide = rand() % 7;
							hexList[i] = NextHexFromMid(start, 9, (Hex_Side)nextHexSide);
							start = hexList[i];
						}
					} break;
					case SDLK_t:
					{
						if(timer->_bound == 1) timer->_bound = 500;
						else timer->_bound = 1;
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

/* set this to any of 512,1024,2048,4096              */
/* the lower it is, the more FPS shown and CPU needed */
#define BUFFER 1024
#define W 1920 /* NEVER make this be less than BUFFER! */
#define H 1080
#define H2 (H/2)
#define H4 (H/4)
#define Y(sample) (((sample)*H)/4/0x7fff)

Sint16 stream[2][BUFFER*2*2];
int len=BUFFER*2*2, done=0, need_refresh=0, bits=0, which=0,
	sample_size=0, position=0, rate=0;
Uint32 flips=0;
Uint32 black,white;
float dy;

static void postmix(void *udata, Uint8 *_stream, int _len)
{
	SDL_Surface *s = (SDL_Surface *)udata;
	position+=_len/sample_size;
	/* fprintf(stderr,"pos=%7.2f seconds \r",position/(float)rate); */
	if(need_refresh)
		return;
	/* save the stream buffer and indicate that we need a redraw */
	len=_len;
	memcpy(stream[(which+1)%2],_stream,len>s->w*4?s->w*4:len);
	which=(which+1)%2;
	need_refresh=1;
}

int clamp(int value)
{
	if(value > 255) return 255;
	if(value < 0) return 0;
	else return value;
}

void refresh(SDL_Surface *s)
{
	int x;
	Sint16 *buf;

	/*fprintf(stderr,"len=%d   \r",len); */

	buf=stream[which];
	need_refresh=0;
	
	SDL_LockSurface(s);

	/* draw the wav from the saved stream buffer */
	for(x=0;x<W*2;x++)
	{
		const int X=x>>1, b=x&1 ,t=H4+H2*b;
		int y1,h1;
		if(buf[x]<0)
		{
			h1=-Y(buf[x]);
			y1=t-h1;
		}
		else
		{
			y1=t;
			h1=Y(buf[x]);
		}
		{
			SDL_Rect r={X,H2*b,1};
			r.h=y1-r.y;
			SDL_FillRect(s,&r, 0x1c1c1c1c);
		}
		{
			SDL_Rect r={X,y1,1,h1};
			uint8 red = clamp(r.h * 2);
			uint8 green = clamp((r.h * 4) % 2);
			uint8 blue = clamp(r.h << 4);
			uint32 color = (0xFF << 24) | (red << 16) | (green << 8) | (blue << 0);
			SDL_FillRect(s,&r,color);
		}
		{
			SDL_Rect r={X,y1+h1,1};
			r.h=H2+H2*b-r.y;
			SDL_FillRect(s,&r, 0x1c1c1c1c);
		}
	}
	SDL_UnlockSurface(s);
	flips++;
}

int main(int argc, char **argv)
{
	printf("Hello world\n");

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

	Mix_Music *music = Mix_LoadMUS("../res/res.wav");
	if(!music)
	{
		printf("Could not load music.\n%s\nExiting.\n", Mix_GetError());
		return -1;
	}

	Mix_VolumeMusic(SDL_MIX_MAXVOLUME);

	if(Mix_PlayMusic(music, 1) == -1)
	{
		printf("Failed to play music.\n");
		return -1;
	}

	int audio_channels = 0, audio_rate = 0;
	Uint16 audio_format;
	Mix_QuerySpec(&audio_rate, &audio_format, &audio_channels);
	int bits = audio_format & 0xFF;
	sample_size = bits / 8 + audio_channels;
	rate = audio_rate;


	uint32 windowFlags = 0;
	// windowFlags |= SDL_WINDOW_FULLSCREEN;
	windowFlags |= SDL_WINDOW_RESIZABLE;
	SDL_Window *window = SDL_CreateWindow("Hex: 0", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
	                                      Window_Width, Window_Height, windowFlags);
	if(!window)
	{
		printf("Could not create window. Exiting.\n");
		printf("%s\n", SDL_GetError());
		return -1;
	}
	SDL_Surface *surface = SDL_GetWindowSurface(window);
	SDL_Renderer *renderer = SDL_CreateSoftwareRenderer(surface);

	SDL_Surface *wavSurface = SDL_CreateRGBSurface(0, 1920, 1080, 32, 
	                                              0x00FF0000,
	                                              0x0000FF00,
	                                              0x000000FF,
	                                              0xFF000000);
	SDL_SetSurfaceBlendMode(wavSurface, SDL_BLENDMODE_BLEND);

	if(!renderer)
	{
		printf("Could not create renderer. Exiting.\n");
		printf("%s\n", SDL_GetError());
		return -1;
	}
	
	SDL_RendererInfo rinfo;
	SDL_GetRendererInfo(renderer, &rinfo);
	printf("Renderer name: %s\n", rinfo.name);
	printf("Texture formats: \n");
	for(int i = 0; i < rinfo.num_texture_formats-1; ++i)
	{
		printf("\t%s,\n", SDL_GetPixelFormatName(rinfo.texture_formats[i]));
	}
	printf("\t%s\n", SDL_GetPixelFormatName(rinfo.texture_formats[rinfo.num_texture_formats-1]));
	printf("End render info\n");

	Mix_SetPostMix(postmix, surface);

	TTF_Init();
	if(!TTF_WasInit())
	{
		printf("Could not init TTF. Exiting.\n");
		printf("%s\n", TTF_GetError());
		return -1;
	}

	TTF_Font *fontConsolas24 = TTF_OpenFont("../res/consolas.ttf", 24);

	SDL_Color white = { 255, 255, 255, 255 };

	Point *plist = createHexGridPoints();

	RectList rlist = createRectList(100);

	Point mid = { (uint32)Window_Width / 2, (uint32)Window_Height / 2 };
	Point start = mid;

	int hexAmount = 10000;
	Point *hexList = (Point *)malloc(hexAmount * sizeof(Point *));
	hexList[0] = start;

	for(int i = 1; i < hexAmount; ++i)
	{
		int nextHexSide = rand() % 7;
		hexList[i] = NextHexFromMid(start, LENGTH, (Hex_Side)nextHexSide);
		start = hexList[i];
	}

	uint32 color1 = 0xFFFF0000;
	uint32 color2 = 0xFF00FF00;

	SDL_Rect originalWindowSize = { Window_Width, Window_Height };

	SDL_Rect scaledWindowSize = { 3456, 1944 };

	Timer ref = CreateTimer(1);
	Timer gen = CreateTimer(500);

	uint64 startClock = SDL_GetTicks();
	do
	{
		HandleEvents(renderer, window, &color1, &color2, hexList, hexAmount, mid, &ref);
		#if 0
		if(CLEAR)
		{
			SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
			SDL_RenderClear(renderer);
		}
		#endif

		if(TickTimer(&ref)) refresh(wavSurface);

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

		// Draw random hex grid
		for (int i = 0; i < hexAmount; ++i)
		{
			DrawHexToSurface(surface, hexList[i], 9, createRandomColor());
		}

		#if 0
		BouncingSquares(surface, rlist);
		PulsingHexes(renderer, surface, plist);
		
		
		#endif

		SDL_BlitSurface(wavSurface, NULL, surface, NULL);
		
		SDL_UpdateWindowSurface(window);

		uint64 ms = SDL_GetTicks() - startClock;
		// Delay the clock until 8 ms are up. This should make the screen render at 125 fps max.
		while(ms < 8)
		{
			ms = SDL_GetTicks() - startClock;
		}
		uint64 fps = 0;
		if(ms > 0) fps = (1.0f/(float32)ms) * 1000.0f;
		char wtextBuffer[128];
		sprintf(wtextBuffer, "Hex: ms/f: %llu, fps: %llu %s", ms, fps, (CLEAR ? "CLEAR" : "NOCLEAR"));
		SDL_SetWindowTitle(window, wtextBuffer);
		startClock = SDL_GetTicks();
	} while(Global_running);

	TTF_CloseFont(fontConsolas24);
	Mix_CloseAudio();
	SDL_FreeSurface(surface);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	printf("Goodbye.\n");

	return 0;
}