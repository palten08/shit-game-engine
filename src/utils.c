#include "../include/utils.h"

double get_delta_time(Uint64 *ticks_now, Uint64 *ticks_last) {
    *ticks_last = *ticks_now;
    *ticks_now = SDL_GetPerformanceCounter();
    return ((*ticks_now - *ticks_last)*1000 / (double)SDL_GetPerformanceFrequency() );
}