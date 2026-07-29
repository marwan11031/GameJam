#include "raylib.h"
#include <stdio.h>

#define WIDTH 800
#define HEIGHT 400

typedef struct {
    float radius;
    float speed;
    Vector2 pos;
    bool active;
} Droplet;



typedef struct {
    Rectangle shape;
    float speed;
    unsigned int score;
} Player;


void spawnDroplet(Droplet droplets[], int maxDroplets) {
    int padding = 20;
    for (int i = 0; i < maxDroplets; i++) {
        if(!droplets[i].active) {
            droplets[i].pos.x = GetRandomValue(0 + padding, WIDTH - padding);
            droplets[i].radius = 10;
            droplets[i].speed = GetRandomValue(170, 250);
            droplets[i].pos.y = 0;
            droplets[i].active = true;
            break;
        }
    }
}

int main() {

    InitWindow(WIDTH, HEIGHT, "Raylib");
    SetTargetFPS(60);
    int score = 0;

    Player p = {
        .shape = (Rectangle) { 400, HEIGHT - 30, 80, 30},
        .speed = 600,
        .score = 0,
    };

    Camera2D camera = {0};
    camera.offset = (Vector2) {
        (float)WIDTH/2, (float)HEIGHT/2
    };
    camera.rotation = 0;
    camera.zoom = 1;
    camera.target = (Vector2) {
        p.shape.x, p.shape.y
    };

    Droplet droplets[30] = {0};

    float timer = 0;
    float spawnTimer = 0;
    float spawnInterval = 1.5;

    while(!WindowShouldClose()) {
        float dt = GetFrameTime();

        timer += dt;
        if (timer >= 15 && spawnInterval > 0.5) {
            spawnInterval -= 0.1;
            timer = 0;
        }

        spawnTimer += dt;
        if(spawnTimer >= spawnInterval) {
            spawnDroplet(droplets, 30);
            spawnTimer = 0;
        }

        if(IsKeyDown(KEY_A)) {
            p.shape.x -= p.speed * dt;
        }

        if(IsKeyDown(KEY_D)) {
            p.shape.x += p.speed * dt;
        }

        if(p.shape.x < 0) {
            p.shape.x = 0;
        }

        if(p.shape.x > WIDTH - p.shape.width) {
            p.shape.x = WIDTH - p.shape.width;
        }


        for(int i = 0; i < 30; i++) {
            if(droplets[i].active) {
                droplets[i].pos.y += droplets[i].speed * dt;
                if (CheckCollisionCircleRec(droplets[i].pos, droplets[i].radius, p.shape)) {
                    droplets[i].active = false;
                    score++;
                }

                if (droplets[i].pos.y > HEIGHT) {
                    droplets[i].active = false;
                }
            }
        }

        //NOTE: Drawing
        BeginDrawing();
        ClearBackground(WHITE);
        DrawRectangle(p.shape.x, p.shape.y, p.shape.width, p.shape.height, DARKGREEN);


        for(int i = 0; i < 30; i++) {
            if (droplets[i].active) {
                DrawCircleV(droplets[i].pos, 10, BLUE);
            }
        }

        char buffer[12];
        snprintf(buffer, sizeof(buffer),"%d", score);
        // printf("spawnTimer: %f\n\n", spawnTimer);
        // printf("Timer: %f\n\n", timer);
        DrawText(buffer, 10, 10, 20, BROWN);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
