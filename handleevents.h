#ifndef HANDLEEVENTS_H
#define HANDLEEVENTS_H

enum View
{
  VIEW_WAVEFORM,
  VIEW_BUFFER
};

static const SDL_Point MOUSERESET = { INT_MIN, INT_MIN };

struct Mouse
{
  int x;
  int y;
  SDL_Point click = MOUSERESET;
  SDL_Point down  = MOUSERESET;
  SDL_Point up    = MOUSERESET;
  bool firstDown  = false;
};

bool insideWaveRect;

bool mouseDown = false;
bool inrect = false;

void HandleEvents(SDL_Renderer *renderer, SDL_Window *window, Mouse *mouse, Mix_Music *music, 
                  float32 *cursor, View *view, SDL_Texture *wavTexture, SDL_Rect *wavRect)
{
  SDL_GetMouseState(&mouse->x, &mouse->y);
  SDL_Event event;

  if(mouseDown)
  {
    if(!mouse->firstDown)
    {
      mouse->click.x = mouse->x;
      mouse->click.y = mouse->y;
      mouse->firstDown = true;
      if(SDL_PointInRect(&mouse->click, wavRect)) inrect = true;
    }
    #if 0
    if(inrect)
    {
      if(mouse->y > mouse->click.y)
      {
        int delta = mouse->y - mouse->click.y;
        float32 perc = ((float)delta / 100.0f) * (Window_Width - 100);
        wavRect->w = perc;
      }
    }
    #endif
  }

  if(SDL_PollEvent(&event))
  {
    switch(event.type)
    {
      case SDL_QUIT:
      {
        Global_running = false;
      } break;
      case SDL_MOUSEMOTION:
      {
      } break;
      case SDL_MOUSEBUTTONDOWN:
      {
        mouseDown = true;
      } break;
      case SDL_MOUSEBUTTONUP:
      {
        mouse->click = MOUSERESET;
        mouse->firstDown = false;
        insideWaveRect = false;
        mouseDown = false;
        inrect = false;
      } break;
      case SDL_MOUSEWHEEL:
      {
        switch(event.wheel.type)
        {
          case SDL_MOUSEWHEEL:
          {
            int amount;
            if(event.wheel.y > 0) amount = 16;
            else amount = -16;
            if(VOLUME < SDL_MIX_MAXVOLUME)
            {
              VOLUME += amount;
            }
            else if(VOLUME > 0)
            {
              VOLUME += amount;
            }
            Mix_VolumeMusic(VOLUME);
          } break;
        }
      }
      case SDL_KEYDOWN:
      {
        SDL_Keycode key = event.key.keysym.sym;
        switch(key)
        {
          case SDLK_ESCAPE:
          {
            Global_running = false;
          } break;
          case SDLK_SPACE:
          {
            Global_paused = !Global_paused;
            if(Mix_PausedMusic()) 
            {
              if(Mix_PlayingMusic())
              {
                Mix_ResumeMusic();
              }
              else
              {
                if(Mix_PlayMusic(music, -1) == -1)
                {
                  printf("Could not play music.\n");
                }
              }
            }
            else
            {
              if(!Mix_PlayingMusic())
              {
                if(Mix_PlayMusic(music, -1) == -1)
                {
                  printf("Could not play music.\n");
                }
              }
              else
              {
                Mix_PauseMusic();
              }
            }
          } break;
          case SDLK_a:
          {
          } break;
          case SDLK_b:
          {
            if(*view == VIEW_WAVEFORM) *view = VIEW_BUFFER;
            else *view = VIEW_WAVEFORM;
          } break;
          case SDLK_c:
          {
          } break;
          case SDLK_d:
          {
          } break;
          case SDLK_e:
          {
          } break;
          case SDLK_f:
          {
          } break;
          case SDLK_g:
          {
          } break;
          case SDLK_h:
          {
          } break;
          case SDLK_i:
          {
          } break;
          case SDLK_j:
          {
          } break;
          case SDLK_k:
          {
          } break;
          case SDLK_l:
          {
          } break;
          case SDLK_m:
          {
          } break;
          case SDLK_n:
          {
          } break;
          case SDLK_o:
          {
          } break;
          case SDLK_p:
          {
          } break;
          case SDLK_q:
          {
          } break;
          case SDLK_r:
          {
          } break;
          case SDLK_s:
          {
          } break;
          case SDLK_t:
          {
          } break;
          case SDLK_u:
          {
          } break;
          case SDLK_v:
          {
            Mix_HaltMusic();
            Global_paused = true;
            *cursor = 10.0f;
          } break;
          case SDLK_w:
          {
          } break;
          case SDLK_x:
          {
          } break;
          case SDLK_y:
          {
          } break;
          case SDLK_z:
          {
          } break;
          case SDLK_UP:
          {

          } break;
          case SDLK_DOWN:
          {

          } break;
          case SDLK_RIGHT:
          {
          } break;
          case SDLK_LEFT:
          {
          } break;
        }
      }
      case SDL_WINDOWEVENT:
      {
        switch(event.window.event)
        {
          case SDL_WINDOWEVENT_MINIMIZED:
          {
            Global_paused = true;
          } break;
        }
      } break;
      case SDL_DROPFILE:
      {
        char *filename = event.drop.file;
        printf("Dropped file: %s\n", filename);
        int len = strlen(filename);
        printf("Len: %d\n", len);
        char ext[3];
        ext[0] = filename[len-3];
        ext[1] = filename[len-2];
        ext[2] = filename[len-1];
        printf("ext: %s\n", ext);
        if(!strcmp(ext, "wav"))
        {
          WAVFile file = WAV_openFile(filename);
          SDL_UpdateTexture(wavTexture, NULL, NULL, 0);
          SDL_Surface  *surface = WAV_createSurface(file, Window_Height - 20);
          SDL_UpdateTexture(wavTexture, NULL, surface->pixels, surface->pitch);
          WAV_closeFile(file);
          int w = 0, h = 0;
          SDL_QueryTexture(wavTexture, NULL, NULL, &w, &h);
          wavRect->x = 10;
          wavRect->y = 10;
          wavRect->w = w;
          wavRect->h = h;
        }
        else
        {
          printf("Not a goddamn wave!!\n");
        }
        SDL_free(filename);
      } break;
    }
  }
}

#endif