#include "../include/utils.h"

double get_delta_time(Uint64 *ticks_now, Uint64 *ticks_last) {
    *ticks_last = *ticks_now;
    *ticks_now = SDL_GetPerformanceCounter();
    return ((*ticks_now - *ticks_last)*1000 / (double)SDL_GetPerformanceFrequency() );
}

double get_instantaneous_frame_rate(Uint64 *ticks_now, Uint64 *ticks_last) {
    double delta_time = get_delta_time(ticks_now, ticks_last);
    return 1000.0 / delta_time;
}