/*
  Resources:
  http://soundfile.sapp.org/doc/WaveFormat/
  http://truelogic.org/wordpress/2015/09/04/parsing-a-wav-file-in-c/
*/

#ifndef WAVE_H
#define WAVE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL2/SDL2_sgfxPrimitives.h"

#include "mathfuncs.h"

// NOTE: Surfaces are in 0xRRGGBBAA format
const uint32 COLOR_LEFTCHANNEL  = 0xFF80FF80; // Light green
const uint32 COLOR_RIGHTCHANNEL = 0xFF8080FF; // Light red

const int16   WAV_MAXSAMPLE   = SHRT_MAX;
const int16   WAV_MINSAMPLE   = SHRT_MIN;
const float32 WAV_MAXSAMPLE_F = 32767.0f;
const float32 WAV_MINSAMPLE_F = -32768.0f;

struct WAVFile
{
  FILE    *file;
  char    *filename;
  // HEADER
  uint8    riffChunk[4];
  uint32   overallSize;
  uint8    wave[4];
  // FORMAT CHUNK
  uint8    fmtChunk[4];
  uint32   fmtLength;
  uint32   fmtType;
  uint32   channels;
  uint32   sampleRate;
  uint32   byteRate;
  uint32   blockAlign;
  uint32   bitsPerSample;
  // DATA CHUNK
  uint8    dataChunk[4];
  uint32   dataSize;
  // COUNTS
  uint64   numSamples;
  uint64   numFrames;
  float64  duration;
  // DATA
  int32 *data;
};

#if 1
// Rectangle method
SDL_Surface *WAV_createSurface(WAVFile wav, int height)
{
  int h = height / 2;
  int slices = Window_Width;
  int chunks = wav.numFrames / slices;

  printf("Chunks: %d\n", chunks);

  SDL_Surface *surface = SDL_CreateRGBSurface(0, chunks, height, 32,
                                              0xFF000000,
                                              0x00FF0000,
                                              0x0000FF00,
                                              0x000000FF);
  if(!surface)
  {
    printf("Could not create wav surface.\n");
    printf("%s\n", SDL_GetError());
    return surface;
  }
  SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_BLEND);
  //SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_NONE);

  int startl = h / 2;
  int startr = surface->h - (h / 2);
  int res = 0;
  int16 lmax = SHRT_MIN;
  int16 rmax = SHRT_MIN;
  int x = 0;
  for(int i = 0; i < wav.numFrames; ++i)
  {
    int32 sample = wav.data[i];
    int16 l = abs(sample & 0xFFFF);
    int16 r = abs(sample >> 16);

    lmax = maximum(l, lmax);
    rmax = maximum(r, rmax);

    if(res == slices)
    {
      int ly = lmax * (h / 2) / WAV_MAXSAMPLE;
      int ry = rmax * (h / 2) / WAV_MAXSAMPLE;

      int llyp = startl + ly;
      int rryp = startr + ry;
      int llyn = startl - ly;
      int rryn = startr - ry;

      svlineColor(surface, x, startl, llyp, 0x80FF80FF);
      svlineColor(surface, x, startl, llyn, 0x80FF80FF);

      svlineColor(surface, x, startr, rryp, 0xFF8080FF);
      svlineColor(surface, x, startr, rryn, 0xFF8080FF);

      lmax = SHRT_MIN;
      rmax = SHRT_MIN;

      ++x;
      res = 0;
    }
    else
    {
      ++res;
    }
  }

  shlineColor(surface, 0, surface->w, h / 2, 0x4A4A4AFF);
  shlineColor(surface, 0, surface->w, h, 0x000000FF);
  shlineColor(surface, 0, surface->w, height - (h / 2), 0x4A4A4AFF);

  return surface;
}
#endif

SDL_Texture *WAV_createTexture(SDL_Renderer *renderer, WAVFile wav, int height)
{
  SDL_Texture *texture = {0};
  SDL_Surface *tmp = WAV_createSurface(wav, height);
  if(tmp) 
  {
    texture = SDL_CreateTextureFromSurface(renderer, tmp);
    SDL_FreeSurface(tmp);
  }
  else 
  {
    printf("Error: Could not create waveform surface.\n");
  }
  return texture;
}

/**
 * Thanks to amit
 * http://truelogic.org/wordpress/2015/09/04/parsing-a-wav-file-in-c/
 * Convert seconds into hh:mm:ss format
 * Params:
 * seconds - seconds value
 * Returns: hms - formatted string
 **/
 char *seconds_to_time(float64 raw_seconds) {
  char *hms;
  int hours, hours_residue, minutes, seconds, milliseconds;
  hms = (char *)malloc(100);
 
  sprintf(hms, "%f", raw_seconds);
 
  hours = (int) raw_seconds/3600;
  hours_residue = (int) raw_seconds % 3600;
  minutes = hours_residue/60;
  seconds = hours_residue % 60;
  milliseconds = 0;
 
  // get the decimal part of raw_seconds to get milliseconds
  char *pos;
  pos = strchr(hms, '.');
  int ipos = (int) (pos - hms);
  char decimalpart[15];
  memset(decimalpart, ' ', sizeof(decimalpart));
  strncpy_s(decimalpart, &hms[ipos+1], 3);
  milliseconds = atoi(decimalpart);
  
  sprintf(hms, "%d:%02d:%02d.%d", hours, minutes, seconds, milliseconds);
  return hms;
}

internal inline const char *getWAVFormatType(uint32 fmtType)
{
  switch(fmtType)
  {
    case  1:  return "PCM";     break;
    case  6:  return "A-law";   break;
    case  7:  return "Mu-law";  break;
    default:  return "Unknown"; break;
  }
}

void printWAVFile(WAVFile wav)
{
  printf("\n<\n  WAVFile: %s\n", wav.filename);
  printf("  Format type: %s\n", getWAVFormatType(wav.fmtType));
  printf("  Channels: %d\n", wav.channels);
  printf("  Sample rate: %d\n", wav.sampleRate);
  printf("  Byte rate: %d\n", wav.byteRate);
  printf("  Block align: %d\n", wav.blockAlign);
  printf("  Bits per sample: %d\n", wav.bitsPerSample);
  printf("  Number of samples: %d\n", wav.numSamples);
  printf("  Duration in seconds: %.4f\n", wav.duration);
  printf("  Duration in time: %s\n", seconds_to_time(wav.duration));
  printf(">\n\n");
}

void printWAVFileFULL(WAVFile wav)
{
  printf("<\nWAVFile: %s\n", wav.filename);
  printf("HEADER:\n");
    printf("\t%c%c%c%c\n", wav.riffChunk[0], wav.riffChunk[1], wav.riffChunk[2], wav.riffChunk[3]);
    printf("\tSize in bytes: %d\n", wav.overallSize);
    printf("\t%c%c%c%c\n", wav.wave[0], wav.wave[1], wav.wave[2], wav.wave[3]);
  printf("FORMAT CHUNK:\n");
    printf("\t%c%c%c%c\n", wav.fmtChunk[0], wav.fmtChunk[1], wav.fmtChunk[2], wav.fmtChunk[3]);
    printf("\tFormat length: %d\n", wav.fmtLength);
    printf("\tFormat type: %s\n", getWAVFormatType(wav.fmtType));
    printf("\tChannels: %d\n", wav.channels);
    printf("\tSample rate: %d\n", wav.sampleRate);
    printf("\tByte rate: %d\n", wav.byteRate);
    printf("\tBlock align: %d\n", wav.blockAlign);
    printf("\tBits per sample: %d\n", wav.bitsPerSample);
  printf("DATA CHUNK:\n");
    printf("\t%c%c%c%c\n", wav.dataChunk[0], wav.dataChunk[1], wav.dataChunk[2], wav.dataChunk[3]);
    printf("\tData size: %d\n", wav.dataSize);
  printf("COUNTS:\n");
    printf("\tNumber of samples: %llu\n", wav.numSamples);
    printf("\tDuration in seconds: %.4f\n", wav.duration);
    printf("\tDuration in time: %s\n", seconds_to_time(wav.duration));
  printf(">\n\n");
}

internal inline uint32 littleToBig2(uint8 buffer[2])
{
  return buffer[0] | (buffer[1] << 8);
}

internal inline uint32 littleToBig4(uint8 buffer[4])
{
  return buffer[0] | (buffer[1] << 8) | (buffer[2] << 16) | (buffer[3] << 24);
}

WAVFile WAV_openFile(const char *filename)
{
  WAVFile wav = {0};
  int len = strlen(filename);
  char *ext = (char *)malloc(3 * sizeof(char));
  ext[0] = filename[len-3];
  ext[1] = filename[len-2];
  ext[2] = filename[len-1];
  if(strcmp(ext, "wav"))
  {
    printf("File does not have a .wav extention.\n");
    return wav;
  }
  free(ext);
  wav.file = fopen(filename, "rb");
  if(wav.file)
  {
    uint8 buffer4[4];
    uint8 buffer2[2];
    wav.filename = (char *)malloc(len * sizeof(char));
    strcpy(wav.filename, filename);
    fread(wav.riffChunk, 4, 1, wav.file);
    if(strcmp((char *)wav.riffChunk, "RIFF"))
    {
      printf("Invalid WAV file. %s\n", filename);
      return wav;
    }
    fread(buffer4, 4, 1, wav.file);
    wav.overallSize = littleToBig4(buffer4);
    fread(wav.wave, 4, 1,  wav.file);
    if(strcmp((char *)wav.wave, "WAVE"))
    {
      printf("Invalid WAV file. %s\n", filename);
      return wav;
    }
    fread(wav.fmtChunk, 4, 1, wav.file);
    fread(buffer4, 4, 1, wav.file);
    wav.fmtLength = littleToBig4(buffer4);
    fread(buffer2, 2, 1, wav.file);
    wav.fmtType = littleToBig2(buffer2);
    fread(buffer2, 2, 1, wav.file);
    wav.channels = littleToBig2(buffer2);
    fread(buffer4, 4, 1, wav.file);
    wav.sampleRate = littleToBig4(buffer4);
    fread(buffer4, 4, 1, wav.file);
    wav.byteRate = littleToBig4(buffer4);
    fread(buffer2, 2, 1, wav.file);
    wav.blockAlign = littleToBig2(buffer2);
    fread(buffer2, 2, 1, wav.file);
    wav.bitsPerSample = littleToBig2(buffer2);
    fread(wav.dataChunk, 4, 1, wav.file);
    fread(buffer4, 4, 1, wav.file);
    wav.dataSize = littleToBig4(buffer4);

    wav.duration = (float64)wav.overallSize / (float64)wav.byteRate;
    wav.numSamples = wav.dataSize / wav.channels;
    wav.numFrames = wav.dataSize / sizeof(int32);

    wav.data = (int32 *)calloc(wav.numFrames, sizeof(int32));
    fread(wav.data, wav.dataSize, sizeof(int8), wav.file);
  }
  else
  {
    printf("Could not open file %s \n", filename);
    return wav;
  }

  return wav;
}

inline void WAV_closeFile(WAVFile wav)
{
  fclose(wav.file);
  free(wav.filename);
  free(wav.data);
}

#endif