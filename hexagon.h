#ifndef HEXAGON_H
#define HEXAGON_H

/*

This is a header file for creating mathematically "correct" hexagons
using integer math. These hexagons can be created by any drawing routine
that allows direct pixel manipulation of a surface.

The implementaion is to give back a structure of

m = midpoint of the starting hexagon
length = the length of the top and bottom sides of the hexagon

l = length - 1
h = l / 2;
f = h / 2;
d = 2 * l

             START                                         MIDDLE RIGHT
      _______length______                               ___________________
     /|                  \                             /                   \
    / |                   \                           /                     \
   /  |                    \                         /                       \
  /   l                     \                       /                         \
 /    |                      \    BOTTOM RIGHT     /                           \
/_____h_______[m]___(l+f)_____\___________________/______((2*(l+f))+length)     \
\              |              /                   \                             /
 \             |             /                     \                           /
  \            |            /                       \                         /
   \           |           /                         \                       /
    \          |          /                           \                     /
     \         |         /                             \___________________/
      \________|________/____________(l+h+f)           /
     /         |         \                            /
    /          |          \                          /
   /           |           \                        /
  /            |            \                      /
 /             |             \                    /
/              d______________\__________________/
\                             /
 \                           /
  \                         /
   \                       /
    \                     /
     \                   /
      \_________________/
            BOTTOM
*/

#include "datatypes.h"
#include "drawing.h"

enum Hex_Side
{
  HEXSIDE_M  = 0,
  HEXSIDE_U  = 1,
  HEXSIDE_UR = 2,
  HEXSIDE_DR = 3,
  HEXSIDE_D  = 4,
  HEXSIDE_DL = 5,
  HEXSIDE_UL = 6,
  HEXSIDE_MR = 7, // SPECIAL CASE. USE ONLY IF NEED NEXT HEX TO THE RIGHT WHEN TILING
  HEXSIDE_ML = 8  // SPECIAL CASE. USE ONLY IF NEED NEXT HEX TO THE LEFT WHEN TILING
};

struct Hexagon
{
  SDL_Point  m;
  int        length;
  uint32     color;
};

SDL_Point NextHexFromMid(Hexagon hex, Hex_Side side)
{
  if(side == HEXSIDE_M) return hex.m;
  SDL_Point p = {0};
  int l = hex.length-1;
  int h = l / 2;
  int f = h / 2;
  int d = 2 * l;
  switch(side)
  {
    case HEXSIDE_U:  p = { hex.m.x, hex.m.y-d }; break;
    case HEXSIDE_UR: p = { hex.m.x+l+h+f, hex.m.y-l }; break;
    case HEXSIDE_DR: p = { hex.m.x+l+h+f, hex.m.y+l }; break;
    case HEXSIDE_D:  p = { hex.m.x, hex.m.y+d }; break;
    case HEXSIDE_DL: p = { hex.m.x-l-h-f, hex.m.y-l }; break;
    case HEXSIDE_UL: p = { hex.m.x-l-h-f, hex.m.y+l }; break;
    case HEXSIDE_MR: p = { hex.m.x+(2*(l+f))+hex.length, hex.m.y }; break;
    case HEXSIDE_ML: p = { hex.m.x-(2*(l+f))-hex.length, hex.m.y }; break;
  }

  return p;
}

void DrawHexToSurface(SDL_Surface *surface, Hexagon hex)
{
  SDL_LockSurface(surface);

  int x = hex.m.x;
  int y = hex.m.y;

  int l = hex.length-1;
  int h = l / 2;
  int f = h / 2;

  hline(surface, x-h, x+h, y+l, hex.color); // TOP
  hline(surface, x-h, x+h, y-l, hex.color); // BOTTOM

  line(surface, x+h, y-l, x+l+f, y, hex.color); // TOP RIGHT
  line(surface, x+l+f, y, x+h, y+l, hex.color); // BOTTOM RIGHT
  line(surface, x-h, y+l, x-l-f, y, hex.color); // BOTTOM LEFT
  line(surface, x-l-f, y, x-h, y-l, hex.color); // TOP LEFT

  SDL_UnlockSurface(surface);
}

#endif