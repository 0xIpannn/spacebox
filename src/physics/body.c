#include "body.h"
#include "physics/constants.h"
#include "raymath.h"
#include <stdio.h>

static unsigned int next_id = 1;

Body body_create(const char *name, Vector3 position, Vector3 velocity,
                 float mass, float radius, Color color) {
  Body body = {.id = next_id++,
               .position = position,
               .velocity = velocity,
               .acceleration = {0},
               .mass = mass,
               .radius = radius,
               .color = color,
               .isActive = true};
  snprintf(body.name, sizeof(body.name), "%s", name);
  return body;
}

void body_orbit(Body *body1, Body *body2, float orbital_speed) {
  Vector3 direction = Vector3Subtract(body1->position, body2->position);
  float distance = Vector3Length(direction);
  if (distance < 0.0001f)
    return;

  Vector3 tangent =
      Vector3Normalize((Vector3){-direction.z, 0.0f, direction.x});
  body1->velocity = Vector3Scale(tangent, orbital_speed);
}

void body_orbit_auto(Body *body1, Body *body2) {
  Vector3 direction = Vector3Subtract(body1->position, body2->position);
  float distance = Vector3Length(direction);
  if (distance < 0.0001f)
    return;

  float orbital_speed = sqrtf(GRAVITATIONAL_CONSTANT * body2->mass / distance);

  Vector3 tangent =
      Vector3Normalize((Vector3){-direction.z, 0.0f, direction.x});
  body1->velocity = Vector3Scale(tangent, orbital_speed);
}
