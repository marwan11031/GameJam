#include "raylib.h"
#include <stdio.h>

#define WIDTH 800
#define HEIGHT 400
#define DROPLETS_MAX 30

typedef enum {
  Normal,
  Golden,
  Toxic,
} DropletType;

typedef enum {
  PLAY,
  START,
  TRANSITION,
} GameState;

typedef struct {
  float radius;
  float speed;
  Vector2 pos;
  bool active;
  DropletType type;
} Droplet;

typedef struct {
  Rectangle shape;
  float speed;
  unsigned score;
} Player;

void spawnDroplet(Droplet droplets[], int maxDroplets) {
  int padding = 20;
  int typeSeed = GetRandomValue(0, 100);

  for (int i = 0; i < maxDroplets; i++) {
    if (!droplets[i].active) {

      if (typeSeed < 5) {
        droplets[i].type = Golden;
      } else if (typeSeed >= 5 && typeSeed <= 20) {
        droplets[i].type = Toxic;
      } else {
        droplets[i].type = Normal;
      }

      droplets[i].pos.x = GetRandomValue(0 + padding, WIDTH - padding);
      droplets[i].radius = 10;
      droplets[i].speed = GetRandomValue(170, 250);
      droplets[i].pos.y = 0;
      droplets[i].active = true;
      break;
    }
  }
}

int main(void) {

  InitWindow(WIDTH, HEIGHT, "Raylib");
  SetTargetFPS(60);

  Player p = {
      .shape = (Rectangle){400, HEIGHT - 30, 80, 30},
      .speed = 600,
      .score = 0,
  };

  Droplet droplets[DROPLETS_MAX] = {0};

  float timer = 0;
  float spawnTimer = 0;
  float spawnInterval = 1.5;

  while (!WindowShouldClose()) {
    float dt = GetFrameTime();

    timer += dt;
    if (timer >= 15 && spawnInterval > 0.5) {
      spawnInterval -= 0.1;
      timer = 0;
    }

    spawnTimer += dt;
    if (spawnTimer >= spawnInterval) {
      spawnDroplet(droplets, 30);
      spawnTimer = 0;
    }

    if (IsKeyDown(KEY_A)) {
      p.shape.x -= p.speed * dt;
    }

    if (IsKeyDown(KEY_D)) {
      p.shape.x += p.speed * dt;
    }

    if (p.shape.x < 0) {
      p.shape.x = 0;
    }

    if (p.shape.x > WIDTH - p.shape.width) {
      p.shape.x = WIDTH - p.shape.width;
    }

    for (int i = 0; i < DROPLETS_MAX; i++) {
      if (droplets[i].active) {
        droplets[i].pos.y += droplets[i].speed * dt;
        if (CheckCollisionCircleRec(droplets[i].pos, droplets[i].radius,
                                    p.shape)) {
          droplets[i].active = false;
          p.score++;
        }

        if (droplets[i].pos.y > HEIGHT) {
          droplets[i].active = false;
        }
      }
    }

    // NOTE: Drawing
    BeginDrawing();
    ClearBackground(WHITE);
    DrawRectangle(p.shape.x, p.shape.y, p.shape.width, p.shape.height,
                  DARKGREEN);

    for (int i = 0; i < DROPLETS_MAX; i++) {
      if (droplets[i].active) {
        if (droplets[i].type == Normal) {
          DrawCircleV(droplets[i].pos, 10, BLUE);
        } else if (droplets[i].type == Toxic) {
          DrawCircleV(droplets[i].pos, 10, GREEN);
        } else {
          DrawCircleV(droplets[i].pos, 10, GOLD);
        }
      }
    }

    char buffer[12];
    snprintf(buffer, sizeof(buffer), "%u", p.score);
    DrawText(buffer, 10, 10, 20, GRAY);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
