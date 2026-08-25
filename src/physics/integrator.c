#include "integrator.h"
#include "raymath.h"

void symplectic_euler(Body *body, float dt) {
  body->velocity =
      Vector3Add(body->velocity, Vector3Scale(body->acceleration, dt));
  body->position = Vector3Add(body->position, Vector3Scale(body->velocity, dt));
}

void leapfrog_kick(Body *body, float halfDt) {
  body->velocity =
      Vector3Add(body->velocity, Vector3Scale(body->acceleration, halfDt));
}

void leapfrog_drift(Body *body, float dt) {
  body->position = Vector3Add(body->position, Vector3Scale(body->velocity, dt));
}
