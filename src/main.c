#include "raylib.h"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

static const int screenWidth = 800;
static const int screenHeight = 450;

static void UpdateDrawFrame(void)
{
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("reentry-minimalism: raylib + WebAssembly pipeline OK", 24, 24, 20, DARKGRAY);
    DrawCircle(150, 240, 50, MAROON);
    DrawRectangle(320, 190, 100, 100, DARKBLUE);
    DrawTriangle((Vector2){ 560, 190 }, (Vector2){ 510, 290 }, (Vector2){ 610, 290 }, DARKGREEN);
    DrawFPS(10, screenHeight - 30);
    EndDrawing();
}

int main(void)
{
    InitWindow(screenWidth, screenHeight, "reentry-minimalism — prototype");

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        UpdateDrawFrame();
    }
#endif
    CloseWindow();
    return 0;
}
