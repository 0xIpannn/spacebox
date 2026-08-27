#include "body.h"
#include "physics/constants.h"
#include "raymath.h"
#include <stdio.h>

static unsigned int next_id = 1;

static void body_zero(Body *body) {
  body->hasAtmosphere = false;
  body->atmosphereThickness = 0.0f;
  body->atmosphereColor = WHITE;
  body->luminosity = 0.0f;
  body->eventHorizonRadius = 0.0f;
}

Body body_create(const char *name, Vector3 position, Vector3 velocity,
                 float mass, float radius, Color color) {
  Body body = {.id = next_id++,
               .type = BODY_PLANET,
               .position = position,
               .velocity = velocity,
               .acceleration = {0},
               .mass = mass,
               .radius = radius,
               .color = color,
               .isActive = true};
  snprintf(body.name, sizeof(body.name), "%s", name);
  body_zero(&body);
  return body;
}

Body body_create_star(const char *name, Vector3 position, Vector3 velocity,
                      float mass, float radius, Color color, float luminosity) {
  Body body = body_create(name, position, velocity, mass, radius, color);
  body.type = BODY_STAR;
  body.luminosity = luminosity;
  return body;
};

Body body_create_planet(const char *name, Vector3 position, Vector3 velocity,
                        float mass, float radius, Color color,
                        bool hasAtmosphere, float atmosphereThickness,
                        Color atmosphereColor) {
  Body body = body_create(name, position, velocity, mass, radius, color);
  body.type = BODY_PLANET;
  body.hasAtmosphere = hasAtmosphere;
  body.atmosphereThickness = atmosphereThickness;
  body.atmosphereColor = atmosphereColor;
  return body;
};

Body body_create_black_hole(const char *name, Vector3 position,
                            Vector3 velocity, float mass, float radius,
                            Color color, float eventHorizonRadius) {
  Body body = body_create(name, position, velocity, mass, radius, color);
  body.type = BODY_BLACK_HOLE;
  body.eventHorizonRadius = eventHorizonRadius;
  return body;
};

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

  float orbital_speed = sqrtf(G * body2->mass / distance);

  Vector3 tangent =
      Vector3Normalize((Vector3){-direction.z, 0.0f, direction.x});
  body1->velocity = Vector3Scale(tangent, orbital_speed);
}
