#pragma once
#include "utils/bodyVector.h"

typedef struct {
  BodyVector bodies;
  float timeMultiplier;
} Simulation;

void simulation_init(Simulation *sim);
void simulation_update(Simulation *sim, float dt);
void simulation_shutdown(Simulation *sim);
