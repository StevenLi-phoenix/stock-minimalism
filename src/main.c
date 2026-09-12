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
    if (IsKeyDown(KEY_SPACE))
    {
        shuttleRotating = true;
    } else {
        shuttleRotating = false;
    }
}

static void UpdateGameState(void)
{
    
    if (shuttleRotation >= 0.0f)
    {
        // reset to nutral position
        shuttleRotation = 0.0f;
    }
    if (shuttleRotating)
    {
        // commanded pitch up
        shuttleRotation -= 180.0f * GetFrameTime();
    } else {
        // pitch down, at lower speed than commanded pitch up
        shuttleRotation += 10.0f * GetFrameTime();
    }
}

static void DrawFrame(void)
{
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawRectangleLinesEx((Rectangle){ 0, 0, (float)screenWidth, (float)screenHeight }, 2, BLACK);
    
    // how to use i18n_T() to draw text?
    // DrawText(i18n_T(STR_PIPELINE_OK), 24, 24, 20, DARKGRAY);

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
