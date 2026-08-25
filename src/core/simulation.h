#pragma once
#include "utils/bodyVector.h"

typedef struct {
  BodyVector bodies;
  float timeMultiplier;
} Simulation;

void simulation_init(Simulation *sim);
void simulation_step(Simulation *sim, float stepDt);
void simulation_shutdown(Simulation *sim);
