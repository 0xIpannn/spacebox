#include "gravity.h"
#include "physics/constants.h"
#include <raymath.h>

Vector3 calculate_grav_force(Body body1, Body body2) {
  Vector3 direction = Vector3Subtract(body2.position, body1.position);
  float distanceSquared = Vector3LengthSqr(direction);

  if (distanceSquared <= 0.0000001f) {
    return Vector3Zero();
  }

  float forceMagnitude =
      (GRAVITATIONAL_CONSTANT * body1.mass * body2.mass) / distanceSquared;
  return Vector3Scale(Vector3Normalize(direction), forceMagnitude);
}

void reset_acceleration(BodyVector bodies) {
  for (int i = 0; i < bodies.count; i++) {
    bodies.data[i].acceleration = Vector3Zero();
  }
}

void apply_gravity(BodyVector bodies) {
  for (int i = 0; i < bodies.count; i++) {
    if (!bodies.data[i].isActive || bodies.data[i].mass == 0.0f)
      continue;

    for (int j = i + 1; j < bodies.count; j++) {
      if (!bodies.data[j].isActive || bodies.data[j].mass == 0.0f)
        continue;

      Vector3 forceThatBe =
          calculate_grav_force(bodies.data[i], bodies.data[j]);

      bodies.data[i].acceleration =
          Vector3Add(bodies.data[i].acceleration,
                     Vector3Scale(forceThatBe, 1.0f / bodies.data[i].mass));
      bodies.data[j].acceleration =
          Vector3Add(bodies.data[j].acceleration,
                     Vector3Scale(forceThatBe, -1.0f / bodies.data[j].mass));
    }
  }
}
