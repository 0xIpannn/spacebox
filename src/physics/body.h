#pragma once
#include "raylib.h"

typedef enum { BODY_PLANET, BODY_STAR, BODY_BLACK_HOLE } BodyType;

typedef struct Body {
  // Shared properties
  unsigned int id;
  char name[64];
  BodyType type;

  Vector3 position;
  Vector3 velocity;
  Vector3 acceleration;

  float mass;
  float radius;

  Color color;
  bool isActive;

  // Planet properties
  bool hasAtmosphere;
  float atmosphereThickness;
  Color atmosphereColor;

  // Star properties
  float luminosity;

  // Black hole properties
  float eventHorizonRadius;
} Body;

Body body_create(const char *name, Vector3 position, Vector3 velocity,
                 float mass, float radius, Color color);

Body body_create_star(const char *name, Vector3 position, Vector3 velocity,
                      float mass, float radius, Color color, float luminosity);
Body body_create_planet(const char *name, Vector3 position, Vector3 velocity,
                        float mass, float radius, Color color,
                        bool hasAtmosphere, float atmosphereThickness,
                        Color atmosphereColor);

void body_orbit(Body *body1, Body *body2, float orbital_speed);
void body_orbit_auto(Body *body1, Body *body2);
