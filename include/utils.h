#pragma once

#include "types.h"

double get_delta_time(Uint64 *ticks_now, Uint64 *ticks_last);

double get_instantaneous_frame_rate(Uint64 *ticks_now, Uint64 *ticks_last);