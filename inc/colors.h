#ifndef COLORS_H
#define COLORS_H

// TODO(alex): Create functions for different pixel modes (RGBA8888, ARGB888, etc)
// TODO(alex): Create uint32 to uint8 and vice versa functions 

// ==================================  STRUCTURES  ===============================================//

struct fColor
{
  float32 r;
  float32 g;
  float32 b;
  float32 a;
};

struct tColor
{
  uint8 r;
  uint8 g;
  uint8 b;
  uint8 a;
};

// ================================== FUNCTION DECLARATIONS ===================================== //

internal uint32 roundFlot32ToUin32(float32 number);
internal uint32 hColorFromFloat(float32 r, float32 g, float32 b, float32 a);
internal fColor fColorFromHex(uint32 color);
internal tColor tColorFromHex(uint32 color);

// ================================== COLOR DECLARATIONS ======================================== //


// CUSTOM COLORS


// CUSTOM TCOLORS


// CUSTOM FCOLORS

#ifdef COLOR_MODE_ABGR8888
// STANDARD COLORS                    0xAABBGGRR (SDL GFX SPECIFIED)
static const uint32 COLOR_WHITE     = 0xFFFFFFFF;
static const uint32 COLOR_BLACK     = 0xFF000000;
static const uint32 COLOR_GREY      = 0xFF808080;
static const uint32 COLOR_LIGHTGREY = 0xFFC0C0C0;
static const uint32 COLOR_DARKGREY  = 0xFF404040;
static const uint32 COLOR_RED       = 0xFF0000FF;
static const uint32 COLOR_GREEN     = 0xFF00FF00;
static const uint32 COLOR_BLUE      = 0xFFFF0000;
static const uint32 COLOR_CYAN      = 0xFFFFFF00;
static const uint32 COLOR_MAGENTA   = 0xFFFF00FF;
static const uint32 COLOR_YELLOW    = 0xFF00FFFF;
#else
static const uint32 COLOR_WHITE     = 0xFFFFFFFF;
static const uint32 COLOR_BLACK     = 0xFF000000;
static const uint32 COLOR_GREY      = 0xFF808080;
static const uint32 COLOR_LIGHTGREY = 0xFFC0C0C0;
static const uint32 COLOR_DARKGREY  = 0xFF404040;
static const uint32 COLOR_RED       = 0xFFFF0000;
static const uint32 COLOR_GREEN     = 0xFF00FF00;
static const uint32 COLOR_BLUE      = 0xFF0000FF;
static const uint32 COLOR_CYAN      = 0xFF00FFFF;
static const uint32 COLOR_MAGENTA   = 0xFFFF00FF;
static const uint32 COLOR_YELLOW    = 0xFFFFFF00;
#endif

// STANDARD TCOLORS
static const tColor tcWhite     = tColorFromHex(COLOR_WHITE);
static const tColor tcBlack     = tColorFromHex(COLOR_BLACK);
static const tColor tcGrey      = tColorFromHex(COLOR_GREY);
static const tColor tcLightGrey = tColorFromHex(COLOR_LIGHTGREY);
static const tColor tcDarkGrey  = tColorFromHex(COLOR_DARKGREY);
static const tColor tcRed       = tColorFromHex(COLOR_RED);
static const tColor tcBlue      = tColorFromHex(COLOR_BLUE);
static const tColor tcGreen     = tColorFromHex(COLOR_GREEN);
static const tColor tcCyan      = tColorFromHex(COLOR_CYAN);
static const tColor tcMagenta   = tColorFromHex(COLOR_MAGENTA);
static const tColor tcYellow    = tColorFromHex(COLOR_YELLOW);

// STANDARD FCOLORS
static const fColor fcWhite     = fColorFromHex(COLOR_WHITE);
static const fColor fcBlack     = fColorFromHex(COLOR_BLACK);
static const fColor fcGrey      = fColorFromHex(COLOR_GREY);
static const fColor fcLightGrey = fColorFromHex(COLOR_LIGHTGREY);
static const fColor fcDarkGrey  = fColorFromHex(COLOR_DARKGREY);
static const fColor fcRed       = fColorFromHex(COLOR_RED);
static const fColor fcBlue      = fColorFromHex(COLOR_BLUE);
static const fColor fcGreen     = fColorFromHex(COLOR_GREEN);
static const fColor fcCyan      = fColorFromHex(COLOR_CYAN);
static const fColor fcMagenta   = fColorFromHex(COLOR_MAGENTA);
static const fColor fcYellow    = fColorFromHex(COLOR_YELLOW);

// ================================== FUNCTION DEFINITIONS ====================================== //

internal uint32 roundFloat32ToUint32(float32 number)
{
  uint32 result = (uint32)(number + 0.5f);
  return result;
}

internal uint32 hColorFromFloat(float32 r, float32 g, float32 b, float32 a)
{
  uint32 result = ((roundFloat32ToUint32(r * 255.0f) << 0) |
                   (roundFloat32ToUint32(g * 255.0f) << 8)  |
                   (roundFloat32ToUint32(b * 255.0f) << 16)  |
                   (roundFloat32ToUint32(a * 255.0f) << 24));
}

internal fColor fColorFromHex(uint32 color)
{
  fColor result = {};
  result.r = (uint8)(color >> 0) / 255.0f;
  result.g = (uint8)(color >> 8)  / 255.0f;
  result.b = (uint8)(color >> 16)  / 255.0f;
  result.a = (uint8)(color >> 24) / 255.0f;

  return result;
}

internal tColor tColorFromHex(uint32 color)
{
  tColor result;
  result.r = (uint8)(color >> 0);
  result.g = (uint8)(color >> 8);
  result.b = (uint8)(color >> 16);
  result.a = (uint8)(color >> 24);
  return result;
}

#endif