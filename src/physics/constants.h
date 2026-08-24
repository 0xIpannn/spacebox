#pragma once

// Normally G is 6.6743e-11 but spacebox use
// this value instead, to scale with the distance unit used.
// (1 Distance Unit = 5e6KM)
#define GRAVITATIONAL_CONSTANT 7.9275e-3f

// to achieve ~25 sec per 1 earth revolution on 1x speed
#define SIM_SPEED 14.61f
