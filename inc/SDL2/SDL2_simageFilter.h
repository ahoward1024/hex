/*

SDL_imageFilter.h: byte-image "filter" routines 

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

#ifndef _SDL_simageFilter_h
#define _SDL_simageFilter_h

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

	/* ---- Function Prototypes */

#ifdef _MSC_VER
#  if defined(DLL_EXPORT) && !defined(LIBSDL_SGFX_DLL_IMPORT)
#    define SDL_SIMAGEFILTER_SCOPE __declspec(dllexport)
#  else
#    ifdef LIBSDL_SGFX_DLL_IMPORT
#      define SDL_SIMAGEFILTER_SCOPE __declspec(dllimport)
#    endif
#  endif
#endif
#ifndef SDL_SIMAGEFILTER_SCOPE
#  define SDL_SIMAGEFILTER_SCOPE extern
#endif

	/* Comments:                                                                           */
	/*  1.) MMX functions work best if all data blocks are aligned on a 32 bytes boundary. */
	/*  2.) Data that is not within an 8 byte boundary is processed using the C routine.   */
	/*  3.) Convolution routines do not have C routines at this time.                      */

	// Detect MMX capability in CPU
	SDL_SIMAGEFILTER_SCOPE int SDL_simageFilterMMXdetect(void);

	// Force use of MMX off (or turn possible use back on)
	SDL_SIMAGEFILTER_SCOPE void SDL_simageFilterMMXoff(void);
	SDL_SIMAGEFILTER_SCOPE void SDL_simageFilterMMXon(void);

	//
	// All routines return:
	//   0   OK
	//  -1   Error (internal error, parameter error)
	//

	//  SDL_imageFilterAdd: D = saturation255(S1 + S2)
	SDL_SIMAGEFILTER_SCOPE int SDL_simageFilterAdd(unsigned char *Src1, unsigned char *Src2, unsigned char *Dest, unsigned int length);

	//  SDL_imageFilterMean: D = S1/2 + S2/2
	SDL_SIMAGEFILTER_SCOPE int SDL_simageFilterMean(unsigned char *Src1, unsigned char *Src2, unsigned char *Dest, unsigned int length);

	//  SDL_imageFilterSub: D = saturation0(S1 - S2)
	SDL_SIMAGEFILTER_SCOPE int SDL_simageFilterSub(unsigned char *Src1, unsigned char *Src2, unsigned char *Dest, unsigned int length);

	//  SDL_imageFilterAbsDiff: D = | S1 - S2 |
	SDL_SIMAGEFILTER_SCOPE int SDL_simageFilterAbsDiff(unsigned char *Src1, unsigned char *Src2, unsigned char *Dest, unsigned int length);

	//  SDL_imageFilterMult: D = saturation(S1 * S2)
	SDL_SIMAGEFILTER_SCOPE int SDL_simageFilterMult(unsigned char *Src1, unsigned char *Src2, unsigned char *Dest, unsigned int length);

	//  SDL_imageFilterMultNor: D = S1 * S2   (non-MMX)
	SDL_SIMAGEFILTER_SCOPE int SDL_simageFilterMultNor(unsigned char *Src1, unsigned char *Src2, unsigned char *Dest, unsigned int length);

	//  SDL_imageFilterMultDivby2: D = saturation255(S1/2 * S2)
	SDL_SIMAGEFILTER_SCOPE int SDL_simageFilterMultDivby2(unsigned char *Src1, unsigned char *Src2, unsigned char *Dest,
		unsigned int length);

	//  SDL_imageFilterMultDivby4: D = saturation255(S1/2 * S2/2)
	SDL_SIMAGEFILTER_SCOPE int SDL_simageFilterMultDivby4(unsigned char *Src1, unsigned char *Src2, unsigned char *Dest,
		unsigned int length);

	//  SDL_imageFilterBitAnd: D = S1 & S2
	SDL_SIMAGEFILTER_SCOPE int SDL_simageFilterBitAnd(unsigned char *Src1, unsigned char *Src2, unsigned char *Dest, unsigned int length);

	//  SDL_imageFilterBitOr: D = S1 | S2
	SDL_SIMAGEFILTER_SCOPE int SDL_simageFilterBitOr(unsigned char *Src1, unsigned char *Src2, unsigned char *Dest, unsigned int length);

	//  SDL_imageFilterDiv: D = S1 / S2   (non-MMX)
	SDL_SIMAGEFILTER_SCOPE int SDL_simageFilterDiv(unsigned char *Src1, unsigned char *Src2, unsigned char *Dest, unsigned int length);

	//  SDL_imageFilterBitNegation: D = !S
	SDL_SIMAGEFILTER_SCOPE int SDL_simageFilterBitNegation(unsigned char *Src1, unsigned char *Dest, unsigned int length);

	//  SDL_imageFilterAddByte: D = saturation255(S + C)
	SDL_SIMAGEFILTER_SCOPE int SDL_simageFilterAddByte(unsigned char *Src1, unsigned char *Dest, unsigned int length, unsigned char C);

	//  SDL_imageFilterAddUint: D = saturation255(S + (uint)C)
	SDL_SIMAGEFILTER_SCOPE int SDL_simageFilterAddUint(unsigned char *Src1, unsigned char *Dest, unsigned int length, unsigned int C);

	//  SDL_imageFilterAddByteToHalf: D = saturation255(S/2 + C)
	SDL_SIMAGEFILTER_SCOPE int SDL_simageFilterAddByteToHalf(unsigned char *Src1, unsigned char *Dest, unsigned int length,
		unsigned char C);

	//  SDL_imageFilterSubByte: D = saturation0(S - C)
	SDL_SIMAGEFILTER_SCOPE int SDL_simageFilterSubByte(unsigned char *Src1, unsigned char *Dest, unsigned int length, unsigned char C);

	//  SDL_imageFilterSubUint: D = saturation0(S - (uint)C)
	SDL_SIMAGEFILTER_SCOPE int SDL_simageFilterSubUint(unsigned char *Src1, unsigned char *Dest, unsigned int length, unsigned int C);

	//  SDL_imageFilterShiftRight: D = saturation0(S >> N)
	SDL_SIMAGEFILTER_SCOPE int SDL_simageFilterShiftRight(unsigned char *Src1, unsigned char *Dest, unsigned int length, unsigned char N);

	//  SDL_imageFilterShiftRightUint: D = saturation0((uint)S >> N)
	SDL_SIMAGEFILTER_SCOPE int SDL_simageFilterShiftRightUint(unsigned char *Src1, unsigned char *Dest, unsigned int length, unsigned char N);

	//  SDL_imageFilterMultByByte: D = saturation255(S * C)
	SDL_SIMAGEFILTER_SCOPE int SDL_simageFilterMultByByte(unsigned char *Src1, unsigned char *Dest, unsigned int length, unsigned char C);

	//  SDL_imageFilterShiftRightAndMultByByte: D = saturation255((S >> N) * C)
	SDL_SIMAGEFILTER_SCOPE int SDL_simageFilterShiftRightAndMultByByte(unsigned char *Src1, unsigned char *Dest, unsigned int length,
		unsigned char N, unsigned char C);

	//  SDL_imageFilterShiftLeftByte: D = (S << N)
	SDL_SIMAGEFILTER_SCOPE int SDL_simageFilterShiftLeftByte(unsigned char *Src1, unsigned char *Dest, unsigned int length,
		unsigned char N);

	//  SDL_imageFilterShiftLeftUint: D = ((uint)S << N)
	SDL_SIMAGEFILTER_SCOPE int SDL_simageFilterShiftLeftUint(unsigned char *Src1, unsigned char *Dest, unsigned int length,
		unsigned char N);

	//  SDL_imageFilterShiftLeft: D = saturation255(S << N)
	SDL_SIMAGEFILTER_SCOPE int SDL_simageFilterShiftLeft(unsigned char *Src1, unsigned char *Dest, unsigned int length, unsigned char N);

	//  SDL_imageFilterBinarizeUsingThreshold: D = S >= T ? 255:0
	SDL_SIMAGEFILTER_SCOPE int SDL_simageFilterBinarizeUsingThreshold(unsigned char *Src1, unsigned char *Dest, unsigned int length,
		unsigned char T);

	//  SDL_imageFilterClipToRange: D = (S >= Tmin) & (S <= Tmax) 255:0
	SDL_SIMAGEFILTER_SCOPE int SDL_simageFilterClipToRange(unsigned char *Src1, unsigned char *Dest, unsigned int length,
		unsigned char Tmin, unsigned char Tmax);

	//  SDL_imageFilterNormalizeLinear: D = saturation255((Nmax - Nmin)/(Cmax - Cmin)*(S - Cmin) + Nmin)
	SDL_SIMAGEFILTER_SCOPE int SDL_simageFilterNormalizeLinear(unsigned char *Src, unsigned char *Dest, unsigned int length, int Cmin,
		int Cmax, int Nmin, int Nmax);

	/* !!! NO C-ROUTINE FOR THESE FUNCTIONS YET !!! */

	//  SDL_imageFilterConvolveKernel3x3Divide: Dij = saturation0and255( ... )
	SDL_SIMAGEFILTER_SCOPE int SDL_simageFilterConvolveKernel3x3Divide(unsigned char *Src, unsigned char *Dest, int rows,
		int columns, signed short *Kernel, unsigned char Divisor);

	//  SDL_imageFilterConvolveKernel5x5Divide: Dij = saturation0and255( ... )
	SDL_SIMAGEFILTER_SCOPE int SDL_simageFilterConvolveKernel5x5Divide(unsigned char *Src, unsigned char *Dest, int rows,
		int columns, signed short *Kernel, unsigned char Divisor);

	//  SDL_imageFilterConvolveKernel7x7Divide: Dij = saturation0and255( ... )
	SDL_SIMAGEFILTER_SCOPE int SDL_simageFilterConvolveKernel7x7Divide(unsigned char *Src, unsigned char *Dest, int rows,
		int columns, signed short *Kernel, unsigned char Divisor);

	//  SDL_imageFilterConvolveKernel9x9Divide: Dij = saturation0and255( ... )
	SDL_SIMAGEFILTER_SCOPE int SDL_simageFilterConvolveKernel9x9Divide(unsigned char *Src, unsigned char *Dest, int rows,
		int columns, signed short *Kernel, unsigned char Divisor);

	//  SDL_imageFilterConvolveKernel3x3ShiftRight: Dij = saturation0and255( ... )
	SDL_SIMAGEFILTER_SCOPE int SDL_simageFilterConvolveKernel3x3ShiftRight(unsigned char *Src, unsigned char *Dest, int rows,
		int columns, signed short *Kernel,
		unsigned char NRightShift);

	//  SDL_imageFilterConvolveKernel5x5ShiftRight: Dij = saturation0and255( ... )
	SDL_SIMAGEFILTER_SCOPE int SDL_simageFilterConvolveKernel5x5ShiftRight(unsigned char *Src, unsigned char *Dest, int rows,
		int columns, signed short *Kernel,
		unsigned char NRightShift);

	//  SDL_imageFilterConvolveKernel7x7ShiftRight: Dij = saturation0and255( ... )
	SDL_SIMAGEFILTER_SCOPE int SDL_simageFilterConvolveKernel7x7ShiftRight(unsigned char *Src, unsigned char *Dest, int rows,
		int columns, signed short *Kernel,
		unsigned char NRightShift);

	//  SDL_imageFilterConvolveKernel9x9ShiftRight: Dij = saturation0and255( ... )
	SDL_SIMAGEFILTER_SCOPE int SDL_simageFilterConvolveKernel9x9ShiftRight(unsigned char *Src, unsigned char *Dest, int rows,
		int columns, signed short *Kernel,
		unsigned char NRightShift);

	//  SDL_imageFilterSobelX: Dij = saturation255( ... )
	SDL_SIMAGEFILTER_SCOPE int SDL_simageFilterSobelX(unsigned char *Src, unsigned char *Dest, int rows, int columns);

	//  SDL_imageFilterSobelXShiftRight: Dij = saturation255( ... )
	SDL_SIMAGEFILTER_SCOPE int SDL_simageFilterSobelXShiftRight(unsigned char *Src, unsigned char *Dest, int rows, int columns,
		unsigned char NRightShift);

	// Align/restore stack to 32 byte boundary -- Functionality untested! --
	SDL_SIMAGEFILTER_SCOPE void SDL_simageFilterAlignStack(void);
	SDL_SIMAGEFILTER_SCOPE void SDL_simageFilterRestoreStack(void);

	/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif

#endif				/* _SDL_imageFilter_h */
