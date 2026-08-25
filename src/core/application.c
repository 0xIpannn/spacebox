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
#include "physics/constants.h"
#include "renderer/renderer.h"
#include "renderer/ui.h"

// fixed timestep constants for simulation
#define PHYS_DT (1.0f / 240.0f)
#define MAX_FRAME_TIME 0.25f

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
  // fixed timestep var
  float accumulator = 0.0f;

  float lastTimeMult = 0.0f;

  // Main game loop
  while (!WindowShouldClose()) {
    float dt = GetFrameTime();
    accumulator += dt;
    if (accumulator > MAX_FRAME_TIME)
      accumulator = MAX_FRAME_TIME;

    while (accumulator >= PHYS_DT) {
      simulation_step(sim, PHYS_DT * SIM_SPEED * sim->timeMultiplier);
      accumulator -= PHYS_DT;
    }
    camera_update(&cc, dt);

    if (IsKeyPressed(KEY_GRAVE)) {
      if (sim->timeMultiplier != 0.0f) {
        lastTimeMult = sim->timeMultiplier;
        sim->timeMultiplier = 0.0f;
      } else {
        sim->timeMultiplier = lastTimeMult;
      }
    }
    if (IsKeyPressed(KEY_ONE)) {
      sim->timeMultiplier = 1.0f;
    }
    if (IsKeyPressed(KEY_TWO)) {
      sim->timeMultiplier = 10.0f;
    }
    if (IsKeyPressed(KEY_THREE)) {
      sim->timeMultiplier = 100.0f;
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      Vector2 mousePos = GetMousePosition();
      selectedBodyId =
          selection_pick_body(&sim->bodies, get_camera(&cc), mousePos);
    };
    Body *selectedBody = vector_get_by_id(&sim->bodies, selectedBodyId);

    BeginDrawing();

    ClearBackground(BLACK);

    BeginMode3D(get_camera(&cc));

    draw_bodies(sim->bodies);
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
