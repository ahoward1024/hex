#ifndef HANDLEEVENTS_H
#define HANDLEEVENTS_H

static const SDL_Point MOUSERESET = { INT_MIN, INT_MIN };

struct Mouse
{
  int x;
  int y;
  SDL_Point click = MOUSERESET;
  bool ctoggle = false;
};

bool insideWaveRect;

void HandleEvents(SDL_Renderer *renderer, SDL_Window *window, Mouse *mouse, Mix_Music *music, 
                  float32 *cursor, SDL_Surface *surface, SDL_Surface *wavSurface, SDL_Rect *wsRect)
{
  SDL_GetMouseState(&mouse->x, &mouse->y);
  SDL_Event event;
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
        if(insideWaveRect)
        {
          if(mouse->y < mouse->click.y)
          {
            if(wsRect->w < wavSurface->w)
            {
              wsRect->w += (mouse->click.y - mouse->y);
            }
          }
          else if(mouse->y > mouse->click.y)
          {
            if(wsRect->w > surface->w - 50)
            {
              wsRect->w -= (mouse->y - mouse->click.y);
            }
          }
        }
      } break;
      case SDL_MOUSEBUTTONDOWN:
      {
        if(!mouse->ctoggle)
        {
          mouse->click = { mouse->x, mouse->y };
          mouse->ctoggle = true;
        }
        if(SDL_PointInRect(&mouse->click, wsRect)) insideWaveRect = true;
      } break;
      case SDL_MOUSEBUTTONUP:
      {
        mouse->click = MOUSERESET;
        mouse->ctoggle = false;
        insideWaveRect = false;
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
              Mix_PauseMusic();
            }
          } break;
          case SDLK_a:
          {
            if(zoom > 0.4)
            {
              zoom -= 0.2;
              SDL_Surface *tmp = zoomSurface(wavSurface, zoom, 1.0f, 0);
              SDL_BlitSurface(tmp, NULL, surface, NULL);
              SDL_FreeSurface(tmp);
            }
          } break;
          case SDLK_b:
          {
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
            if(zoom < 4.0)
            {
              zoom += 0.2;
              SDL_Surface *tmp = zoomSurface(wavSurface, zoom, 1.0f, 0);
              SDL_BlitSurface(tmp, NULL, surface, NULL);
              SDL_FreeSurface(tmp);
            }
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
    }
  }
}

#endif