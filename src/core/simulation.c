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
  apply_gravity(sim->bodies);
}

void simulation_step(Simulation *sim, float stepDt) {
  // using leapfrog integrator
  // first kick the velocity by half a step
  for (size_t i = 0; i < sim->bodies.count; i++) {
    leapfrog_kick(&sim->bodies.data[i], stepDt * 0.5f);
  }
  // drift the position by a full step
  for (size_t i = 0; i < sim->bodies.count; i++) {
    leapfrog_drift(&sim->bodies.data[i], stepDt);
  }
  // refresh the acceleration after the position update
  reset_acceleration(sim->bodies);
  apply_gravity(sim->bodies);
  // second kick the velocity by half a step
  for (int i = 0; i < sim->bodies.count; i++) {
    leapfrog_kick(&sim->bodies.data[i], stepDt * 0.5f);
  }
}

void simulation_step_symplectic(Simulation *sim, float stepDt) {
  reset_acceleration(sim->bodies);
  apply_gravity(sim->bodies);
  for (size_t i = 0; i < sim->bodies.count; i++) {
    symplectic_euler(&sim->bodies.data[i], stepDt);
  }
}

void simulation_shutdown(Simulation *sim) { vector_free(&sim->bodies); }
