#ifndef SDL_COLORS_H
#define SDL_COLORS_H

// TODO(alex): ??? Conversions from uint32 to SDL_Color, etc ???

// STANDARD COLORS RGBA

static const SDL_Color SDLC_white     = { 255, 255, 255, 255 };
static const SDL_Color SDLC_black     = {   0,   0,   0, 255 };
static const SDL_Color SDLC_grey      = { 128, 128, 128, 255 };
static const SDL_Color SDLC_lightgrey = { 192, 192, 192, 255 };
static const SDL_Color SDLC_darkgrey  = {  64,  64,  64, 255 };
static const SDL_Color SDLC_red       = { 255,   0,   0, 255 };
static const SDL_Color SDLC_green     = {   0, 255,   0, 255 };
static const SDL_Color SDLC_blue      = {   0,   0, 255, 255 };
static const SDL_Color SDLC_cyan      = {   0, 255, 255, 255 };
static const SDL_Color SDLC_magenta   = { 255,   0, 255, 255 };
static const SDL_Color SDLC_yellow    = { 255, 255,   0, 255 };

// DEFINE ANY CUSTOM SDL_Color STRUCTS AFTER THIS LINE

#endif