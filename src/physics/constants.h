#pragma once

// Sim unit
#define DISTANCE_UNIT 5.0e6f
#define DISTANCE_UNIT_METERS 5.0e9f

#define SOLAR_MASS 1.989e30f
#define MASS_UNIT_KILOGRAMS (SOLAR_MASS) // 1 mass unit = 1 solar mass

#define TIME_UNIT 1.0f
#define TIME_UNIT_SECONDS 86400.0f // 1 time unit = 1 day

// Real life constants
#define G_REAL 6.67430e-11f
#define C_REAL 299792458.0f

// Sim constants
// G_SIM = G_REAL * MASS_UNIT_KILOGRAMS * DISANCE_UNIT_METERS^2 /
// (TIME_UNIT_SECONDS^3)
#define G 7.9279f

// C_SIM = C_REAL * TIME_UNIT_SECONDS / DISTANCE_UNIT_METERS
#define C 5180.41f

#define EARTH_PERIOD_UNIT 365.0f
#define SECONDS_PER_PERIOD 30.0f
#define SIM_SPEED                                                              \
  (EARTH_PERIOD_UNIT / SECONDS_PER_PERIOD) // 1 year in 30 seconds
