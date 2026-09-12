#include "raylib.h"

#include "config.h"
#include "i18n.h"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

// ALSO remember to update itch io display viewpoint
static const int screenWidth = 1280;
static const int screenHeight = 720;

// TODO(stock-minimalism): short press = buy, long hold = sell; draw the
// order stack going up/down, colored red/green. See README.md for the concept.

static void HandleInput(void)
{
    (void)Config_IsPitchUpCommanded; // placeholder until the new input mapping lands
}

static void UpdateGameState(void)
{
}

static void DrawFrame(void)
{
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawRectangleLinesEx((Rectangle){ 0, 0, (float)screenWidth, (float)screenHeight }, 2, BLACK);

    // how to use i18n_T() to draw text?
    // DrawText(i18n_T(STR_PIPELINE_OK), 24, 24, 20, DARKGRAY);

    DrawFPS(10, screenHeight - 30);
    EndDrawing();
}

static void LoadAssets(void)
{
}

static void UnloadAssets(void)
{
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
    InitWindow(screenWidth, screenHeight, "stock-minimalism — prototype");
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
