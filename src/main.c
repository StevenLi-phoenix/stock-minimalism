#include "raylib.h"

#include "config.h"
#include "gamelogic.h"
#include "utils.h"
#include "i18n.h"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

bool debug = true;

// ALSO remember to update itch io display viewpoint
static const int screenWidth = 1280;
static const int screenHeight = 720;

static GameViewDTO gameView;

// debug helper
static const int verticalSpacing = 40;
static const int verticalHeight = 40;
static void DrawStatRow(int rowIndex, int barWidth, Color barColor, const char *text)
{
    int y = verticalSpacing * rowIndex;
    DrawRectangle(24, y, barWidth, verticalHeight, barColor);
    DrawText(text, 24, y, verticalHeight, DARKGRAY);
}

static void HandleInput(void)
{
    PlayerActionDTO action = ResolvePlayerAction(&gameView, IsControlKeyPressed(), GetFrameTime());
    ApplyPlayerAction(&gameView, action);
}

static void StepGameState(void)
{
    UpdateGameState(&gameView, GetFrameTime());
}

static void DrawFrame(void)
{

    // TODO(stock-minimalism): draw the order stack going up/down, colored red/green.
    // how to use i18n_T() to draw text?
    // DrawText(i18n_T(STR_PIPELINE_OK), 24, 24, 20, DARKGRAY);

    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawRectangleLinesEx((Rectangle){ 0, 0, (float)screenWidth, (float)screenHeight }, 2, BLACK);
    

    if (debug) {
        // test drawing current stock/portfolio stats:
        int verticalIndex = 1; // start at y = 40
        DrawStatRow(verticalIndex++, (int)(RatioClamped(gameView.time_remaining, gameView.round_seconds) * (screenWidth - 48)), gameView.time_remaining < 10.0f ? RED : GREEN,TextFormat("%s%d", i18n_T(STR_CURRENT_STEP), gameView.price_index));
        DrawStatRow(verticalIndex++, (int)gameView.current_price, BLUE,TextFormat("%s%.2f", i18n_T(STR_CURRENT_PRICE), gameView.current_price));
        DrawStatRow(verticalIndex++, (int)gameView.cash, BLUE,TextFormat("%s%.2f", i18n_T(STR_CASH), gameView.cash));
        DrawStatRow(verticalIndex++, gameView.shares, BLUE,TextFormat("%s%d", i18n_T(STR_SHARES), gameView.shares));
        DrawStatRow(verticalIndex++, (int)gameView.portfolio_value, GREEN,TextFormat("%s%.2f", i18n_T(STR_PORTFOLIO_VALUE), gameView.portfolio_value));
        DrawStatRow(verticalIndex++, (int)(gameView.hold_duration * 100.0f), gameView.hold_duration > CONFIG_LONG_HOLD_THRESHOLD ? RED : (gameView.holding ? GREEN : BLUE),TextFormat("%s%s%s%.2f", i18n_T(STR_IS_HOLDING_DOWN), gameView.holding ? "true" : "false",i18n_T(STR_PLAYER_HOLD_DURATION), gameView.hold_duration));

        if (gameView.failed) {
            DrawStatRow(verticalIndex++, screenWidth - 48, RED, i18n_T(STR_GAME_OVER));
        }
    }

    // End of drawing, show FPS in the bottom left corner
    DrawFPS(10, screenHeight - 30);
    EndDrawing();
}

static void Initgame(void)
{
    TraceLog(LOG_INFO, "Stage Initgame");
    InitGameState(&gameView, 0, CONFIG_STARTING_CASH, CONFIG_ROUND_SECONDS); // seed 0 = pick a fresh one
    // i18n_SetLanguage(LANG_ZH);
}

static void LoadAssets(void)
{
    TraceLog(LOG_INFO, "Stage LoadAssets");
}

static void UnloadAssets(void)
{
    TraceLog(LOG_INFO, "Stage UnloadAssets");
}

// emscripten_set_main_loop requires 
static void UpdateDrawFrame(void)
{
    TraceLog(LOG_DEBUG, "Stage main step");
    HandleInput();
    StepGameState();
    DrawFrame();
}

int main(void)
{
    InitWindow(screenWidth, screenHeight, "stock-minimalism — prototype");
    LoadAssets();
    Initgame();

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
