#ifndef HEXLIST_H
#define HEXLIST_H

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

#if 0
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
#endif

#if 0
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
#endif

#endif