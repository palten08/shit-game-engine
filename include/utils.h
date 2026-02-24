#pragma once

#include "../include/types.h"
#include <SDL2/SDL.h>
#include <stdio.h>

double get_delta_time(Uint64 *ticks_now, Uint64 *ticks_last);

double get_instantaneous_frame_rate(Uint64 *ticks_now, Uint64 *ticks_last);