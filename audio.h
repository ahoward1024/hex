#ifndef AUDIO_H
#define AUDIO_h

#if 0

// >>> Adapted from: http://jcatki.no-ip.org:8080/SDL_mixer/demos/sdlwav.c
/* set this to any of 512,1024,2048,4096              */
/* the lower it is, the more FPS shown and CPU needed */
#define BUFFER 1024
#define W Window_Width /* NEVER make this be less than BUFFER! */
#define H Window_Height
#define H2 (H/2)
#define H4 (H/4)
#define Y(sample) (((sample)*H)/4/0x7FFF)

Sint16 stream[2][BUFFER*2*2];
int len = BUFFER * 2 * 2, done = 0, bits = 0, which = 0, sample_size = 0, position = 0, rate = 0;
float dy;
int mmax = 0;

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

void posNegWaveform(SDL_Surface *s, uint8 alpha, TTF_Font *font)
{
  int x;
  Sint16 *buf;

  /*fprintf(stderr,"len=%d   \r",len); */

  buf=stream[which];
  
  SDL_LockSurface(s);

  /* draw the wav from the saved stream buffer */
  SDL_Rect r;
  SDL_FillRect(s, NULL, 0x00000000);
  int max = 0;
  #if 1
  for(x = 0; x < (W * 2); x++)
  {
    const int X = x >> 1, b = x & 1, t = H4 + H2 * b;
    int y1,h1;
    int top = 10;
    if(buf[x] < 0)
    {
      if(-buf[x] > max) max = -buf[x];
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
      if(buf[x] > max) max = buf[x];
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
  #endif
  int y = 0;
  {
    #if 1
    y = (max)*Window_Height/0x7FFF;
    SDL_Rect peak = { Window_Width / 2 - 10, Window_Height - y - 5, 20, y };
    SDL_FillRect(s, &peak, 0xFF000000);
    peak = { Window_Width / 2 - 5, Window_Height - y, 10, y };
    uint8 red = colorClamp(0x00);
    uint8 blue = colorClamp(0x00);
    uint8 green = colorClamp(y);
    uint32 color = (0xFF << 24) | (red << 16) | (green << 8) | (blue << 0);
    SDL_FillRect(s, &peak, color);
    if(max > mmax) mmax = max;
    y = (mmax)*Window_Height/0x7FFF;
    peak = { Window_Width / 2 - 5, Window_Height - y, 10, 3 };
    SDL_FillRect(s, &peak, 0xFFFF0000);
    #endif
  }
  SDL_UnlockSurface(s);
  char num[32];
  sprintf(num, "%d", mmax);
  SDL_Color white = { 255, 255, 255, 255 };
  DrawTextToSurface(s, Window_Width / 2 + 10, Window_Height - y - 10, num, font, white);
}
// <<< Adapted from: http://jcatki.no-ip.org:8080/SDL_mixer/demos/sdlwav.c

#endif


#endif