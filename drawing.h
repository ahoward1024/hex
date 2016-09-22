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

const char *getBlendModeString(SDL_Surface *surface)
{
  SDL_BlendMode bm;
  SDL_GetSurfaceBlendMode(surface, &bm);
  if(bm == SDL_BLENDMODE_NONE) return "NONE";
  else if(bm == SDL_BLENDMODE_BLEND) return "BLEND";
  else if(bm == SDL_BLENDMODE_ADD) return "ADD";
  else if(bm == SDL_BLENDMODE_MOD) return "MOD";
  else return "???";
}

void drawTextToSurface(SDL_Surface *dest, int x, int y, const char *text, 
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