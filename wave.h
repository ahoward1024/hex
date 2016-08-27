/*
  Resources:
  http://soundfile.sapp.org/doc/WaveFormat/
  http://truelogic.org/wordpress/2015/09/04/parsing-a-wav-file-in-c/
*/

#ifndef WAVE_H
#define WAVE_H

#include <stdio.h>
#include <stdlib.h>

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
  uint64   sampleSize;
  float64  duration;
  int16    maxLeft;
  int16    minLeft;
  int16    maxRight;
  int16    minRight;
  // STREAM
  int32   *data;
};

SDL_Surface *createWaveformSurface(WAVFile wav, int height)
{
  int h = (height / 2);
  int slices = 2048;
  int chunks = wav.numFrames / slices;
  printf("chunks: %d\n", chunks);

  int *lcMaxCache = (int *)calloc(chunks, sizeof(int));
  int *lcMinCache = (int *)calloc(chunks, sizeof(int));
  int *rcMaxCache = (int *)calloc(chunks, sizeof(int));
  int *rcMinCache = (int *)calloc(chunks, sizeof(int));

  int16 lmax = SHRT_MIN;
  int16 rmax = SHRT_MIN;
  int16 lmin = SHRT_MAX;
  int16 rmin = SHRT_MAX;

  int inc = 0;
  int rset = 0;
  for(int i = 0; i < wav.numFrames; ++i)
  {
    int32 s = wav.data[i];
    // The wave format is as follows:
    // 0x UB LB
    //    LC RC
    // But because of little endianness we need to flip these so the left channel becomes
    // the lower bits, and the right channel the upper bits.
    int16 l = s & 0xFFFF;
    int16 r = s >> 16;
    // Thanks! https://graphics.stanford.edu/~seander/bithacks.html
    lmax = l ^ ((l ^ lmax) & -(l < lmax));
    rmax = r ^ ((r ^ rmax) & -(r < rmax));
    lmin = lmin ^ ((l ^ lmin) & -(l < lmin));
    rmin = rmin ^ ((r ^ rmin) & -(r < rmin));
    if(rset == slices)
    {
      lcMaxCache[inc] = lmax * (h / 2) / wav.maxLeft;
      lcMinCache[inc] = lmin * (h / 2) / wav.minLeft;
      rcMaxCache[inc] = rmax * (h / 2) / wav.maxRight;
      rcMinCache[inc] = rmin * (h / 2) / wav.minRight;

      lmax = SHRT_MIN;
      rmax = SHRT_MIN;
      lmin = SHRT_MAX;
      rmin = SHRT_MAX;

      ++inc;
      rset = 0;
    }
    else
    {
      ++rset;
    }
  }

  SDL_Surface *surface = SDL_CreateRGBSurface(0, inc, height, 32, 
                                              0x00FF0000,
                                              0x0000FF00,
                                              0x000000FF,
                                              0xFF000000);
  if(!surface)
  {
    printf("Could not create wav surface.\n");
    printf("%s\n", SDL_GetError());
    return surface;
  }
  SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_NONE);
  SDL_FillRect(surface, NULL, 0xFF1F1F1F);

  for(int x = 0; x < surface->w; ++x)
  {
    int16 lpmax = lcMaxCache[x];
    int16 lpmin = lcMinCache[x];
    int16 rpmax = rcMaxCache[x];
    int16 rpmin = rcMinCache[x];

    vline(surface, (h / 2) + lpmax, (h / 2) - lpmin, x, COLOR_LEFTCHANNEL);
    vline(surface, (surface->h - (h / 2)) + rpmax, (surface->h - (h / 2)) - rpmin, x, 
          COLOR_RIGHTCHANNEL);
  }

  hline(surface, 0, surface->w, surface->h / 2, COLOR_BLACK);
  hline(surface, 0, surface->w, surface->h / 4, COLOR_LIGHTLINE);
  hline(surface, 0, surface->w, surface->h - (surface->h / 4), COLOR_LIGHTLINE);

  free(lcMaxCache);
  free(lcMinCache);
  free(rcMaxCache);
  free(rcMinCache);

  return surface;
}

/**
 * Convert seconds into hh:mm:ss format
 * Params:
 *  seconds - seconds value
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
  strncpy(decimalpart, &hms[ipos+1], 3);
  milliseconds = atoi(decimalpart);
  
  sprintf(hms, "%d:%02d:%02d.%d", hours, minutes, seconds, milliseconds);
  return hms;
}

inline const char *getWAVFormatType(uint32 fmtType)
{
  switch(fmtType)
  {
    case  1:  return "PCM";     break;
    case  6:  return "A-law";   break;
    case  7:  return "Mu-law";  break;
    default:  return "Unknown"; break;
  }
}

inline void printWAVFile(WAVFile wav)
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
    printf("\tNumber of samples: %d\n", wav.numSamples);
    printf("\tSample size: %d\n", wav.sampleSize);
    printf("\tDuration in seconds: %.4f\n", wav.duration);
    printf("\tDuration in time: %s\n", seconds_to_time(wav.duration));
    printf("\tMax left sample: %d\n", wav.maxLeft);
    printf("\tMin left sample: %d\n", wav.minLeft);
    printf("\tMax right sample: %d\n", wav.maxRight);
    printf("\tMin right sample: %d\n", wav.minRight);
  printf(">\n\n");
}

inline uint32 littleToBig2(uint8 buffer[2])
{
  return buffer[0] | (buffer[1] << 8);
}

inline uint32 littleToBig4(uint8 buffer[4])
{
  return buffer[0] | (buffer[1] << 8) | (buffer[2] << 16) | (buffer[3] << 24);
}

WAVFile openWAVFile(const char *filename)
{
  WAVFile wav = {0};
  wav.file = fopen(filename, "rb");
  if(wav.file)
  {
    uint8 buffer4[4];
    uint8 buffer2[2];
    wav.filename = (char *)malloc(strlen(filename) * sizeof(char));
    strcpy(wav.filename, filename);
    fread(wav.riffChunk, 4, 1, wav.file);
    fread(buffer4, 4, 1, wav.file);
    wav.overallSize = littleToBig4(buffer4);
    fread(wav.wave, 4, 1,  wav.file);
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

    wav.sampleSize = (wav.channels * wav.bitsPerSample) / 8;
    wav.duration = (float64)wav.overallSize / (float64)wav.byteRate;
    wav.numSamples = wav.dataSize / wav.channels;
    wav.numFrames = wav.dataSize / 4;

    wav.data = (int32 *)calloc(wav.numFrames, sizeof(int32));
    fread(wav.data, wav.dataSize, sizeof(int8), wav.file);

    wav.maxLeft = SHRT_MIN;
    wav.minLeft = SHRT_MAX;
    wav.maxLeft = SHRT_MIN;
    wav.minRight = SHRT_MAX;
    for(int i = 0; i < wav.numFrames; ++i)
    {
      int32 s = wav.data[i];
      int16 l = s >> 16;
      int16 r = s & 0xFFFF;
      // Thanks! https://graphics.stanford.edu/~seander/bithacks.html
      wav.maxLeft = l ^ ((l ^ wav.maxLeft) & -(l < wav.maxLeft));
      wav.minLeft = wav.minLeft ^ ((l ^ wav.minLeft) & -(l < wav.minLeft));
      wav.maxRight = r ^ ((r ^ wav.maxRight) & -(l < wav.maxRight));
      wav.minRight = wav.minRight ^ ((l ^ wav.minRight) & -(l < wav.minRight));
    }
    assert(wav.maxLeft > 0);
    assert(wav.minLeft < 0);
    assert(wav.maxRight > 0);
    assert(wav.minRight < 0);
  }
  else
  {
    printf("Could not open file %s \n", filename);
  }

  return wav;
}

inline void closeWAVFile(WAVFile wav)
{
  fclose(wav.file);
  free(wav.filename);
  free(wav.data);
}

#endif