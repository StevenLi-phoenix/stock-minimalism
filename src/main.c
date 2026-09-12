#include "raylib.h"

#include "i18n.h"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

// ALSO remember to update itch io display viewpoint
static const int screenWidth = 1280;
static const int screenHeight = 720;

static Texture2D shuttleTexture; // SpaceShuttle.png is 968x341 
static const Rectangle shuttleBounds = { 0, 0, 968, 341 };

static bool shuttleRotating = false;
static float shuttleRotation = 0.0f;

static void HandleInput(void)
{
    if (IsKeyPressed(KEY_SPACE))
    {
        shuttleRotating = !shuttleRotating;
        TraceLog(LOG_INFO, "Shuttle rotation %s", shuttleRotating ? "started" : "stopped");
    }
}

static void UpdateGameState(void)
{
    if (shuttleRotating)
    {
        shuttleRotation += 180.0f * GetFrameTime();
    }
}

static void DrawFrame(void)
{
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawRectangle(0, 0, screenWidth, screenHeight, LIGHTGRAY);
    DrawRectangle(2, 2, screenWidth-4, screenHeight-4, RAYWHITE);
    DrawText(i18n_T(STR_PIPELINE_OK), 24, 24, 20, DARKGRAY);
    DrawCircle(150, 240, 50, MAROON);
    DrawRectangle(320, 190, 100, 100, DARKBLUE);
    DrawTriangle((Vector2){ 560, 190 }, (Vector2){ 510, 290 }, (Vector2){ 610, 290 }, DARKGREEN);

    if (shuttleTexture.id != 0)
    {
        Rectangle source = { 0, 0, (float)shuttleTexture.width, (float)shuttleTexture.height };
        Vector2 origin = { shuttleBounds.width / 2.0f, shuttleBounds.height / 2.0f };
        Rectangle dest = {
            shuttleBounds.x + origin.x,
            shuttleBounds.y + origin.y,
            shuttleBounds.width,
            shuttleBounds.height,
        };
        DrawTexturePro(shuttleTexture, source, dest, origin, shuttleRotation, WHITE);
    }

    DrawFPS(10, screenHeight - 30);
    EndDrawing();
}

static void LoadAssets(void)
{
    shuttleTexture = LoadTexture("assets/SpaceShuttle.png");
    if (shuttleTexture.id == 0)
    {
        TraceLog(LOG_WARNING, "Failed to load assets/SpaceShuttle.png");
    }
}

static void UnloadAssets(void)
{
    UnloadTexture(shuttleTexture);
}

// emscripten_set_main_loop requires 
static void UpdateDrawFrame(void)
{
    HandleInput();
    UpdateGameState();
    DrawFrame();
}

int main(void)
{
    InitWindow(screenWidth, screenHeight, "reentry-minimalism — prototype");
    LoadAssets();

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        UpdateDrawFrame();
    }
#endif
    UnloadAssets();
    CloseWindow();
    return 0;
}
