#pragma once
#include "raylib.h"

typedef struct {
  Camera3D cam;

  Vector3 movement;
  Vector3 rotation;
  float zoom;

  float movementSpeed;
  float rotationSpeed;
  float zoomMultiplier;

  Vector2 lastCursorLocation;
} CameraController;

void camera_init(CameraController *cc);
void camera_update(CameraController *cc, float dt);
Camera3D get_camera(CameraController *cc);
