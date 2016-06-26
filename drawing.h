#ifndef DRAWING_H
#define DRAWING_H

#include "datatypes.h"

inline void clear(SDL_Renderer *renderer, uint32 color)
{
  uint8 *c = (uint8 *)&color;
  SDL_SetRenderDrawColor(renderer, c[0], c[1], c[2], c[3]);
  SDL_RenderClear(renderer);
}

inline void clear(SDL_Renderer *renderer)
{
  SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
  SDL_RenderClear(renderer);
}

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

#endif