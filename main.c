#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>

#define WIDTH 800
#define HEIGHT 400
#define DROPLETS_MAX 25

typedef enum {
  Normal,
  Toxic,
} DropletType;

typedef enum {
  Title,
  Play,
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

Player *InitPlayer() {
  Player *p = malloc(sizeof(*p));
  p->shape = (Rectangle){400, HEIGHT - 30, 80, 30};
  p->speed = 600;
  p->score = 0;
  p->tex = LoadTexture("assets/Bucket.png");
  p->source = (Rectangle){0, 0, p->tex.width, p->tex.height};

  return p;
}

void SpawnDroplet(Droplet droplets[], int maxDroplets, Texture2D dropTex) {
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
      droplets[i].pos.y = 0;

      droplets[i].speed = GetRandomValue(170, 250);
      droplets[i].active = true;
      droplets[i].tex = dropTex;

      droplets[i].source = (Rectangle){.x = 0,
                                       .y = 0,
                                       .width = (float)droplets[i].tex.width,
                                       .height = (float)droplets[i].tex.height};
      break;
    }
  }
}

void UpdateSpawner(Droplet droplets[], Texture2D dropTex, float dt) {
  static float timer = 0;
  static float spawnTimer = 0;
  static float spawnInterval = 1.5f;

  timer += dt;
  if (timer >= 10 && spawnInterval > 0.5f) {
    spawnInterval -= 0.2f;
    timer = 0;
  }

  spawnTimer += dt;
  if (spawnTimer >= spawnInterval) {
    SpawnDroplet(droplets, DROPLETS_MAX, dropTex);
    spawnTimer = 0;
  }
}

void UpdatePlayer(Player *p, float dt) {
  if (IsKeyDown(KEY_A)) {
    p->shape.x -= p->speed * dt;
  }
  if (IsKeyDown(KEY_D)) {
    p->shape.x += p->speed * dt;
  }
  if (p->shape.x < 0) {
    p->shape.x = 0;
  }
  if (p->shape.x > WIDTH - p->shape.width) {
    p->shape.x = WIDTH - p->shape.width;
  }
}

void UpdateDroplets(Droplet droplets[], float dt) {
  for (int i = 0; i < DROPLETS_MAX; i++) {
    if (droplets[i].active) {
      droplets[i].pos.y += droplets[i].speed * dt;

      if (droplets[i].pos.y > HEIGHT) {
        droplets[i].active = false;
      }
    }
  }
}

void ResolveCollisions(Player *p, Droplet droplets[]) {
  for (int i = 0; i < DROPLETS_MAX; i++) {
    if (droplets[i].active) {
      if (CheckCollisionRecs(droplets[i].pos, p->shape)) {
        droplets[i].active = false;

        switch (droplets[i].type) {
        case Toxic:
          if (p->score < 5) {
            p->score = 0;
          } else {
            p->score -= 5;
          }
          break;
        case Normal:
          p->score++;
          break;
        }
      }
    }
  }
}

void UpdateGame(Player *p, Droplet droplets[], Texture2D dropTex, float dt) {
  UpdateSpawner(droplets, dropTex, dt);
  UpdatePlayer(p, dt);
  UpdateDroplets(droplets, dt);
  ResolveCollisions(p, droplets);
}

void DrawTitle(Texture2D background) {
  DrawTexturePro(background,
                 (Rectangle){0, 0, background.width, background.height},
                 (Rectangle){0, 0, WIDTH, HEIGHT}, (Vector2){0, 0}, 0, WHITE);
  DrawText("Press Enter to play...", 300, 200, 20, WHITE);
}

void DrawGame(Player *p, Droplet droplets[]) {
  ClearBackground(WHITE);
  DrawTexturePro(p->tex, p->source, p->shape, (Vector2){0, 0}, 0, WHITE);

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
  snprintf(buffer, sizeof(buffer), "%u", p->score);
  DrawText(buffer, 10, 10, 20, GRAY);
}

int main(void) {
  InitWindow(WIDTH, HEIGHT, "Bucket & Droplets");
  SetTargetFPS(60);

  Player *p = InitPlayer();
  Texture2D dropTex = LoadTexture("assets/Droplit.png");
  Texture2D background = LoadTexture("assets/background.jpg");
  Droplet *droplets = calloc(DROPLETS_MAX, sizeof(*droplets));

  GameState state = Title;

  while (!WindowShouldClose()) {

    float dt = GetFrameTime();

    switch (state) {
    case Title: {
      if (IsKeyDown(KEY_ENTER))
        state = Play;
    } break;
    case Play: {
      UpdateGame(p, droplets, dropTex, dt);
    } break;

    default:
      break;
    }

    BeginDrawing();
    switch (state) {
    case Title: {
      DrawTitle(background);
    } break;
    case Play: {
      DrawGame(p, droplets);
    } break;

    default:
      break;
    }
    EndDrawing();
  }

  UnloadTexture(p->tex);
  UnloadTexture(dropTex);
  UnloadTexture(background);
  free(droplets);
  free(p);

  CloseWindow();
  return 0;
}
