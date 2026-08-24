// C Libraries
#include "math.h"
#include "stdlib.h"

// Raylib Libraries
#include "raylib.h"
#include "rlgl.h"

// Application Libraries
#include "core/application.h"
#include "core/camera.h"
#include "core/selection.h"
#include "core/simulation.h"
#include "renderer/renderer.h"
#include "renderer/ui.h"

void app_run() {
  const int screenWidth = 1600;
  const int screenHeight = 800;

  SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
  InitWindow(screenWidth, screenHeight, "SpaceBox");

  SetTargetFPS(144);

  Simulation *sim = malloc(sizeof(Simulation));
  CameraController cc;
  UIController ui;

  simulation_init(sim);
  camera_init(&cc);
  ui_init(&ui);

  int selectedBodyId = 0;
  Body *selectedBody = nullptr;

  // Main game loop
  while (!WindowShouldClose()) {
    float dt = GetFrameTime();
    simulation_update(sim, dt);
    camera_update(&cc);

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      Vector2 mousePos = GetMousePosition();
      selectedBodyId =
          selection_pick_body(&sim->bodies, get_camera(&cc), mousePos);
      selectedBody = vector_get_by_id(&sim->bodies, selectedBodyId);
    };

    BeginDrawing();

    ClearBackground(BLACK);

    BeginMode3D(get_camera(&cc));

    draw_bodies(sim->bodies, sim->bodies.count);
    DrawGrid(500, 10.0f);

    if (selectedBodyId != 0) {
      draw_body_outline(selectedBody, WHITE);
    }

    EndMode3D();

    if (selectedBodyId != 0) {
      // Draw body data
      Vector3 textWorldPos = selectedBody->position;
      textWorldPos.y += fminf(selectedBody->radius * 2.0f, 20.0f);
      Vector2 textScreenPos = GetWorldToScreen(textWorldPos, get_camera(&cc));
      ui_draw_body_data(ui.regularFont, selectedBody, textScreenPos);
    }

    EndDrawing();
  }

  CloseWindow();
  ui_shutdown(&ui);
  simulation_shutdown(sim);
  free(sim);
}
