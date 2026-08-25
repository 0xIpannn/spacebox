#pragma once

// Normally G is 6.6743e-11 but spacebox use
// this value instead, to scale with the distance unit used.
// (1 Distance Unit = 5e6KM)
#define GRAVITATIONAL_CONSTANT 7.9275e-3f

#define EARTH_PERIOD_UNIT 365.0f
#define SECONDS_PER_PERIOD 30.0f
#define SIM_SPEED                                                              \
  (EARTH_PERIOD_UNIT / SECONDS_PER_PERIOD) // 1 year in 30 seconds
