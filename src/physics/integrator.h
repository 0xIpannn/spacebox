#pragma once
#include "physics/body.h"

void symplectic_euler(Body *body, float dt);
void leapfrog_kick(Body *body, float halfDt);
void leapfrog_drift(Body *body, float dt);
