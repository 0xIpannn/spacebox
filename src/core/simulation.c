#include "simulation.h"
#include "physics/gravity.h"
#include "physics/integrator.h"
#include "scenario/solarSystem.h"
#include "utils/bodyVector.h"
#include <raylib.h>
#include <stddef.h>

void simulation_init(Simulation *sim) {
  sim->timeMultiplier = 1.0f;
  vector_init(&sim->bodies, 10);
  solar_system_init(&sim->bodies);
}

void simulation_step(Simulation *sim, float stepDt) {
  reset_acceleration(sim->bodies);
  apply_gravity(sim->bodies);
  for (size_t i = 0; i < sim->bodies.count; i++) {
    symplectic_euler(&sim->bodies.data[i], stepDt);
  }
}

void simulation_shutdown(Simulation *sim) { vector_free(&sim->bodies); }
