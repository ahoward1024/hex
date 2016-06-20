#ifndef TIMER_H
#define TIMER_H

typedef struct {
  uint64_t       _start;
  uint64_t       _ticks;
  uint64_t       _bound;
  bool           _isRunning;
} Timer;

inline Timer CreateTimer(uint64_t bound) {
  Timer timer = { 0, 0, bound, false };
  return timer;
}

inline bool TickTimer(Timer *timer) {
  if (timer->_isRunning) {
    timer->_ticks = SDL_GetTicks() - timer->_start;
    if (timer->_ticks >= timer->_bound) {
      timer->_isRunning = false;
      timer->_start = 0;
      timer->_ticks = 0;
      return true;
    }
  } else {
    timer->_start = SDL_GetTicks();
    timer->_isRunning = true;
  }
  return false;
}


#endif /* TIMER_H */