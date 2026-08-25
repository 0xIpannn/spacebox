#include "camera.h"
#include <raylib.h>
#include <raymath.h>

static void handle_camera_movement(CameraController *cc) {
  float movementSpeed = cc->movementSpeed * GetFrameTime();
  if (IsKeyDown(KEY_LEFT_SHIFT))
    movementSpeed *= 2.0f;
  if (IsKeyDown(KEY_W))
    cc->movement.x = movementSpeed;
  if (IsKeyDown(KEY_S))
    cc->movement.x -= movementSpeed;
  if (IsKeyDown(KEY_A))
    cc->movement.y -= movementSpeed;
  if (IsKeyDown(KEY_D))
    cc->movement.y += movementSpeed;
}

static void handle_camera_rotation(CameraController *cc) {
  Vector2 delta = GetMouseDelta();

  if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
    cc->lastCursorLocation = GetMousePosition();
  }

  if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
    HideCursor();
    cc->rotation.x += delta.x;
    cc->rotation.y += delta.y;
    SetMousePosition(cc->lastCursorLocation.x, cc->lastCursorLocation.y);
  }

  if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
    ShowCursor();
  }

  cc->rotation = Vector3Scale(cc->rotation, cc->rotationSpeed);
}

static void handle_camera_zoom(CameraController *cc) {
  cc->zoom -= GetMouseWheelMove() * cc->zoomMultiplier;

  if (IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON)) {
    cc->zoom = 0.0f;
  }
}

void camera_init(CameraController *cc) {
  cc->cam = (Camera3D){0};
  cc->cam.position = (Vector3){0.0f, 20.0f, 40.0f};
  cc->cam.target = (Vector3){0.0f, 0.0f, 0.0f};
  cc->cam.up = (Vector3){0.0f, 1.0f, 0.0f};
  cc->cam.fovy = 45.0f;
  cc->cam.projection = CAMERA_PERSPECTIVE;

  cc->movementSpeed = 20.0f;
  cc->rotationSpeed = 0.1f;
  cc->zoomMultiplier = 5.0f;
}

void camera_update(CameraController *cc) {
  cc->movement = (Vector3){0.0f, 0.0f, 0.0f};
  cc->rotation = (Vector3){0.0f, 0.0f, 0.0f};
  cc->zoom = 0.0f;

  handle_camera_movement(cc);
  handle_camera_rotation(cc);
  handle_camera_zoom(cc);

  UpdateCameraPro(&cc->cam, cc->movement, cc->rotation, cc->zoom);
}

Camera3D get_camera(CameraController *cc) { return cc->cam; }
