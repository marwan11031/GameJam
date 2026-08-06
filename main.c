#include "raylib.h"
#include <stdio.h>

#define WIDTH 800
#define HEIGHT 400
#define DROPLETS_MAX 30

typedef enum {
  Normal,
  Toxic,
} DropletType;

typedef enum {
  PLAY,
  START,
  TRANSITION,
} GameState;

typedef struct {
  float speed;
  Rectangle pos;
  bool active;
  DropletType type;
  Texture2D tex;
  Rectangle source;
} Droplet;

typedef struct {
  Rectangle shape;
  float speed;
  unsigned score;

  Texture2D tex;
  Rectangle source;
} Player;

void spawnDroplet(Droplet droplets[], int maxDroplets) {
  int padding = 20;
  int typeSeed = GetRandomValue(0, 100);

  for (int i = 0; i < maxDroplets; i++) {
    if (!droplets[i].active) {

      if (typeSeed >= 0 && typeSeed <= 15) {
        droplets[i].type = Toxic;
      } else {
        droplets[i].type = Normal;
      }

      droplets[i].pos.width = 15;
      droplets[i].pos.height = 15;

      droplets[i].pos.x = GetRandomValue(0 + padding, WIDTH - padding);
      droplets[i].speed = GetRandomValue(170, 250);
      droplets[i].pos.y = 0;
      droplets[i].active = true;
      droplets[i].tex = LoadTexture("assets/Droplit.png");
      droplets[i].source = (Rectangle){.x = 0,
                                       .y = 0,
                                       .width = droplets[i].tex.width,
                                       droplets[i].tex.height};
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
      .tex = LoadTexture("assets/Bucket.png"),
  };

  p.source = (Rectangle){0, 0, (float)p.tex.width, (float)p.tex.height};

  Droplet droplets[DROPLETS_MAX] = {0};

  Texture2D background = LoadTexture("assets/background.jpg");
  Rectangle source = (Rectangle){
      .x = 0,
      .y = 0,
      .width = background.width,
      .height = background.height,
  };

  Rectangle dest = (Rectangle){
      .x = 0,
      .y = 0,
      .width = WIDTH,
      .height = HEIGHT,
  };

  float timer = 0;
  float spawnTimer = 0;
  float spawnInterval = 1.5;

  while (!WindowShouldClose()) {
    float dt = GetFrameTime();

    timer += dt;
    if (timer >= 10 && spawnInterval > 0.5) {
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
        if (CheckCollisionRecs(droplets[i].pos, p.shape)) {
          droplets[i].active = false;

          switch (droplets[i].type) {
          case Toxic: {
            if (p.score < 5) {
              p.score = 0;
            } else {
              p.score -= 5;
            }

          } break;
          case Normal: {
            p.score++;
          } break;

          default:
            break;
          }
        }

        if (droplets[i].pos.y > HEIGHT) {
          droplets[i].active = false;
        }
      }
    }

    // NOTE: Drawing
    BeginDrawing();
    ClearBackground(WHITE);
    // DrawTexturePro(background, source, dest, (Vector2){0, 0}, 0, WHITE);

    DrawTexturePro(p.tex, p.source, p.shape, (Vector2){0, 0}, 0, WHITE);

    for (int i = 0; i < DROPLETS_MAX; i++) {
      if (droplets[i].active) {
        if (droplets[i].type == Normal) {
          DrawTexturePro(droplets[i].tex, droplets[i].source, droplets[i].pos,
                         (Vector2){0, 0}, 0, WHITE);
        } else if (droplets[i].type == Toxic) {
          DrawTexturePro(droplets[i].tex, droplets[i].source, droplets[i].pos,
                         (Vector2){0, 0}, 0, GREEN);
        }
      }
    }

    char buffer[12];
    snprintf(buffer, sizeof(buffer), "%u", p.score);
    DrawText(buffer, 10, 10, 20, GRAY);

    EndDrawing();
  }

  UnloadTexture(p.tex);
  CloseWindow();
  return 0;
}
