/* 

SDL_gfxPrimitives.h: graphics primitives for SDL

Copyright (C) 2001-2012  Andreas Schiffler

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution.

Andreas Schiffler -- aschiffler at ferzkopp dot net

*/

#ifndef _SDL_sgfxPrimitives_h
#define _SDL_sgfxPrimitives_h

#include <math.h>
#ifndef M_PI
#define M_PI	3.1415926535897932384626433832795
#endif

#include "SDL.h"

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

	/* ----- Versioning */

#define SDL_SGFXPRIMITIVES_MAJOR	2
#define SDL_SGFXPRIMITIVES_MINOR	0
#define SDL_SGFXPRIMITIVES_MICRO	25


	/* ---- Function Prototypes */

#ifdef _MSC_VER
#  if defined(DLL_EXPORT) && !defined(LIBSDL_SGFX_DLL_IMPORT)
#    define SDL_SGFXPRIMITIVES_SCOPE __declspec(dllexport)
#  else
#    ifdef LIBSDL_GFX_DLL_IMPORT
#      define SDL_GFXPRIMITIVES_SCOPE __declspec(dllimport)
#    endif
#  endif
#endif
#ifndef SDL_SGFXPRIMITIVES_SCOPE
#  define SDL_SGFXPRIMITIVES_SCOPE extern
#endif

	/* Note: all ___Color routines expect the color to be in format 0xRRGGBBAA */

	/* Pixel */

	SDL_SGFXPRIMITIVES_SCOPE int spixelColor(SDL_Surface * dst, Sint16 x, Sint16 y, Uint32 color);
	SDL_SGFXPRIMITIVES_SCOPE int spixelRGBA(SDL_Surface * dst, Sint16 x, Sint16 y, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

	/* Horizontal line */

	SDL_SGFXPRIMITIVES_SCOPE int shlineColor(SDL_Surface * dst, Sint16 x1, Sint16 x2, Sint16 y, Uint32 color);
	SDL_SGFXPRIMITIVES_SCOPE int shlineRGBA(SDL_Surface * dst, Sint16 x1, Sint16 x2, Sint16 y, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

	/* Vertical line */

	SDL_SGFXPRIMITIVES_SCOPE int svlineColor(SDL_Surface * dst, Sint16 x, Sint16 y1, Sint16 y2, Uint32 color);
	SDL_SGFXPRIMITIVES_SCOPE int svlineRGBA(SDL_Surface * dst, Sint16 x, Sint16 y1, Sint16 y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

	/* Rectangle */

	SDL_SGFXPRIMITIVES_SCOPE int srectangleColor(SDL_Surface * dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color);
	SDL_SGFXPRIMITIVES_SCOPE int srectangleRGBA(SDL_Surface * dst, Sint16 x1, Sint16 y1,
		Sint16 x2, Sint16 y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

	/* Rounded-Corner Rectangle */

	SDL_SGFXPRIMITIVES_SCOPE int sroundedRectangleColor(SDL_Surface * dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Sint16 rad, Uint32 color);
	SDL_SGFXPRIMITIVES_SCOPE int sroundedRectangleRGBA(SDL_Surface * dst, Sint16 x1, Sint16 y1,
		Sint16 x2, Sint16 y2, Sint16 rad, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

	/* Filled rectangle (Box) */

	SDL_SGFXPRIMITIVES_SCOPE int sboxColor(SDL_Surface * dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color);
	SDL_SGFXPRIMITIVES_SCOPE int sboxRGBA(SDL_Surface * dst, Sint16 x1, Sint16 y1, Sint16 x2,
		Sint16 y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

	/* Rounded-Corner Filled rectangle (Box) */

	SDL_SGFXPRIMITIVES_SCOPE int sroundedBoxColor(SDL_Surface * dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Sint16 rad, Uint32 color);
	SDL_SGFXPRIMITIVES_SCOPE int sroundedBoxRGBA(SDL_Surface * dst, Sint16 x1, Sint16 y1, Sint16 x2,
		Sint16 y2, Sint16 rad, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

	/* Line */

	SDL_SGFXPRIMITIVES_SCOPE int slineColor(SDL_Surface * dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color);
	SDL_SGFXPRIMITIVES_SCOPE int slineRGBA(SDL_Surface * dst, Sint16 x1, Sint16 y1,
		Sint16 x2, Sint16 y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

	/* AA Line */

	SDL_SGFXPRIMITIVES_SCOPE int saalineColor(SDL_Surface * dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color);
	SDL_SGFXPRIMITIVES_SCOPE int saalineRGBA(SDL_Surface * dst, Sint16 x1, Sint16 y1,
		Sint16 x2, Sint16 y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

	/* Thick Line */
	SDL_SGFXPRIMITIVES_SCOPE int sthickLineColor(SDL_Surface * dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, 
		Uint8 width, Uint32 color);
	SDL_SGFXPRIMITIVES_SCOPE int sthickLineRGBA(SDL_Surface * dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, 
		Uint8 width, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

	/* Circle */

	SDL_SGFXPRIMITIVES_SCOPE int scircleColor(SDL_Surface * dst, Sint16 x, Sint16 y, Sint16 rad, Uint32 color);
	SDL_SGFXPRIMITIVES_SCOPE int scircleRGBA(SDL_Surface * dst, Sint16 x, Sint16 y, Sint16 rad, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

	/* Arc */

	SDL_SGFXPRIMITIVES_SCOPE int sarcColor(SDL_Surface * dst, Sint16 x, Sint16 y, Sint16 rad, Sint16 start, Sint16 end, Uint32 color);
	SDL_SGFXPRIMITIVES_SCOPE int sarcRGBA(SDL_Surface * dst, Sint16 x, Sint16 y, Sint16 rad, Sint16 start, Sint16 end, 
		Uint8 r, Uint8 g, Uint8 b, Uint8 a);

	/* AA Circle */

	SDL_SGFXPRIMITIVES_SCOPE int saacircleColor(SDL_Surface * dst, Sint16 x, Sint16 y, Sint16 rad, Uint32 color);
	SDL_SGFXPRIMITIVES_SCOPE int saacircleRGBA(SDL_Surface * dst, Sint16 x, Sint16 y,
		Sint16 rad, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

	/* Filled Circle */

	SDL_SGFXPRIMITIVES_SCOPE int sfilledCircleColor(SDL_Surface * dst, Sint16 x, Sint16 y, Sint16 r, Uint32 color);
	SDL_SGFXPRIMITIVES_SCOPE int sfilledCircleRGBA(SDL_Surface * dst, Sint16 x, Sint16 y,
		Sint16 rad, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

	/* Ellipse */

	SDL_SGFXPRIMITIVES_SCOPE int sellipseColor(SDL_Surface * dst, Sint16 x, Sint16 y, Sint16 rx, Sint16 ry, Uint32 color);
	SDL_SGFXPRIMITIVES_SCOPE int sellipseRGBA(SDL_Surface * dst, Sint16 x, Sint16 y,
		Sint16 rx, Sint16 ry, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

	/* AA Ellipse */

	SDL_SGFXPRIMITIVES_SCOPE int saaellipseColor(SDL_Surface * dst, Sint16 x, Sint16 y, Sint16 rx, Sint16 ry, Uint32 color);
	SDL_SGFXPRIMITIVES_SCOPE int saaellipseRGBA(SDL_Surface * dst, Sint16 x, Sint16 y,
		Sint16 rx, Sint16 ry, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

	/* Filled Ellipse */

	SDL_SGFXPRIMITIVES_SCOPE int sfilledEllipseColor(SDL_Surface * dst, Sint16 x, Sint16 y, Sint16 rx, Sint16 ry, Uint32 color);
	SDL_SGFXPRIMITIVES_SCOPE int sfilledEllipseRGBA(SDL_Surface * dst, Sint16 x, Sint16 y,
		Sint16 rx, Sint16 ry, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

	/* Pie */

	SDL_SGFXPRIMITIVES_SCOPE int spieColor(SDL_Surface * dst, Sint16 x, Sint16 y, Sint16 rad,
		Sint16 start, Sint16 end, Uint32 color);
	SDL_SGFXPRIMITIVES_SCOPE int spieRGBA(SDL_Surface * dst, Sint16 x, Sint16 y, Sint16 rad,
		Sint16 start, Sint16 end, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

	/* Filled Pie */

	SDL_SGFXPRIMITIVES_SCOPE int sfilledPieColor(SDL_Surface * dst, Sint16 x, Sint16 y, Sint16 rad,
		Sint16 start, Sint16 end, Uint32 color);
	SDL_SGFXPRIMITIVES_SCOPE int sfilledPieRGBA(SDL_Surface * dst, Sint16 x, Sint16 y, Sint16 rad,
		Sint16 start, Sint16 end, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

	/* Trigon */

	SDL_SGFXPRIMITIVES_SCOPE int strigonColor(SDL_Surface * dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Sint16 x3, Sint16 y3, Uint32 color);
	SDL_SGFXPRIMITIVES_SCOPE int strigonRGBA(SDL_Surface * dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Sint16 x3, Sint16 y3,
		Uint8 r, Uint8 g, Uint8 b, Uint8 a);

	/* AA-Trigon */

	SDL_SGFXPRIMITIVES_SCOPE int saatrigonColor(SDL_Surface * dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Sint16 x3, Sint16 y3, Uint32 color);
	SDL_SGFXPRIMITIVES_SCOPE int saatrigonRGBA(SDL_Surface * dst,  Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Sint16 x3, Sint16 y3,
		Uint8 r, Uint8 g, Uint8 b, Uint8 a);

	/* Filled Trigon */

	SDL_SGFXPRIMITIVES_SCOPE int sfilledTrigonColor(SDL_Surface * dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Sint16 x3, Sint16 y3, Uint32 color);
	SDL_SGFXPRIMITIVES_SCOPE int sfilledTrigonRGBA(SDL_Surface * dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Sint16 x3, Sint16 y3,
		Uint8 r, Uint8 g, Uint8 b, Uint8 a);

	/* Polygon */

	SDL_SGFXPRIMITIVES_SCOPE int spolygonColor(SDL_Surface * dst, const Sint16 * vx, const Sint16 * vy, int n, Uint32 color);
	SDL_SGFXPRIMITIVES_SCOPE int spolygonRGBA(SDL_Surface * dst, const Sint16 * vx, const Sint16 * vy,
		int n, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

	/* AA-Polygon */

	SDL_SGFXPRIMITIVES_SCOPE int saapolygonColor(SDL_Surface * dst, const Sint16 * vx, const Sint16 * vy, int n, Uint32 color);
	SDL_SGFXPRIMITIVES_SCOPE int saapolygonRGBA(SDL_Surface * dst, const Sint16 * vx, const Sint16 * vy,
		int n, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

	/* Filled Polygon */

	SDL_SGFXPRIMITIVES_SCOPE int sfilledPolygonColor(SDL_Surface * dst, const Sint16 * vx, const Sint16 * vy, int n, Uint32 color);
	SDL_SGFXPRIMITIVES_SCOPE int sfilledPolygonRGBA(SDL_Surface * dst, const Sint16 * vx,
		const Sint16 * vy, int n, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
	SDL_SGFXPRIMITIVES_SCOPE int stexturedPolygon(SDL_Surface * dst, const Sint16 * vx, const Sint16 * vy, int n, SDL_Surface * texture,int texture_dx,int texture_dy);

	/* (Note: These MT versions are required for multi-threaded operation.) */

	SDL_SGFXPRIMITIVES_SCOPE int sfilledPolygonColorMT(SDL_Surface * dst, const Sint16 * vx, const Sint16 * vy, int n, Uint32 color, int **polyInts, int *polyAllocated);
	SDL_SGFXPRIMITIVES_SCOPE int sfilledPolygonRGBAMT(SDL_Surface * dst, const Sint16 * vx,
		const Sint16 * vy, int n, Uint8 r, Uint8 g, Uint8 b, Uint8 a,
		int **polyInts, int *polyAllocated);
	SDL_SGFXPRIMITIVES_SCOPE int stexturedPolygonMT(SDL_Surface * dst, const Sint16 * vx, const Sint16 * vy, int n, SDL_Surface * texture,int texture_dx,int texture_dy, int **polyInts, int *polyAllocated);

	/* Bezier */

	SDL_SGFXPRIMITIVES_SCOPE int sbezierColor(SDL_Surface * dst, const Sint16 * vx, const Sint16 * vy, int n, int s, Uint32 color);
	SDL_SGFXPRIMITIVES_SCOPE int sbezierRGBA(SDL_Surface * dst, const Sint16 * vx, const Sint16 * vy,
		int n, int s, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

	/* Characters/Strings */

	SDL_SGFXPRIMITIVES_SCOPE void sgfxPrimitivesSetFont(const void *fontdata, Uint32 cw, Uint32 ch);
	SDL_SGFXPRIMITIVES_SCOPE void sgfxPrimitivesSetFontRotation(Uint32 rotation);
	SDL_SGFXPRIMITIVES_SCOPE int scharacterColor(SDL_Surface * dst, Sint16 x, Sint16 y, char c, Uint32 color);
	SDL_SGFXPRIMITIVES_SCOPE int scharacterRGBA(SDL_Surface * dst, Sint16 x, Sint16 y, char c, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
	SDL_SGFXPRIMITIVES_SCOPE int sstringColor(SDL_Surface * dst, Sint16 x, Sint16 y, const char *s, Uint32 color);
	SDL_SGFXPRIMITIVES_SCOPE int sstringRGBA(SDL_Surface * dst, Sint16 x, Sint16 y, const char *s, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

	/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif

#endif				/* _SDL_gfxPrimitives_h */
