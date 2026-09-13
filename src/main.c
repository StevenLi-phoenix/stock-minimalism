#include "math.h"

#include "raylib.h"

#include "config.h"
#include "gamelogic.h"
#include "utils.h"
#include "i18n.h"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

bool debug = false;

// ALSO remember to update itch io display viewpoint
static const int screenWidth = 1280;
static const int screenHeight = 720;

static const int fontSize_L = 30;
static const int fontSize_M = 25;

static GameViewDTO gameView;

static const int chartVisibleBars = 50;
static const float chartWidthFraction = 0.7f;
static const float chartMargin = 12.0f;
static const float chartTopPadding = 86.0f;
static const float chartHeightFraction = 0.6f;

static const int chartAxisFontSize = 16;
static const float chartAxisGutter = 72.0f;
static const int chartAxisLines = 5;
static const float chartScaleStepFraction = 0.5f;

static int CenterText(const char *txt, int fontSize) 
{ 
    return (screenWidth - MeasureText(txt, fontSize)) / 2; 
}

static Rectangle GetChartArea(void)
{
    float x = chartMargin + chartAxisGutter;
    float width = ((float)screenWidth - 2.0f * chartMargin - chartAxisGutter) * chartWidthFraction;
    return (Rectangle){ x, chartTopPadding, width, screenHeight * chartHeightFraction };
}

static float PriceToY(float price, Rectangle area, float low, float span)
{
    return area.y + (1.0f - (price - low) / span) * area.height;
}

static void DrawChartBars(Rectangle area)
{
    if (gameView.price_index < 1) return;

    int last = gameView.price_index - 1;
    int first = last - chartVisibleBars + 1;
    if (first < 0) first = 0;
    int count = last - first + 1;

    float low = gameView.price_history[first];
    float high = low;
    float sum = 0.0f;
    int sampleCount = 0;
    for (int i = first; i <= last + 1; i++) {
        float price = gameView.price_history[i];
        if (price < low) low = price;
        if (price > high) high = price;
        sum += price;
        sampleCount++;
    }
    float center = sum / (float)sampleCount;

    float step = gameView.price_history[0] * chartScaleStepFraction;
    if (step < 1.0f) step = 1.0f;
    float required = fmaxf(high - center, center - low);
    float steps = ceilf(required / step);
    if (steps < 1.0f) steps = 1.0f;
    float halfSpan = steps * step;

    low = center - halfSpan;
    high = center + halfSpan;
    float span = high - low;

    // Price axis
    for (int i = 0; i < chartAxisLines; i++) {
        float t = (float)i / (float)(chartAxisLines - 1);
        float price = low + t * span;
        float y = PriceToY(price, area, low, span);

        DrawLineEx((Vector2){ area.x, y }, (Vector2){ area.x + area.width, y }, 1.0f, Fade(GRAY, 0.25f));

        const char *label = TextFormat("%.0f", price);
        DrawText(label,
                 (int)area.x - 8 - MeasureText(label, chartAxisFontSize),
                 (int)y - chartAxisFontSize / 2,
                 chartAxisFontSize, GRAY);
    }

    float sectionWidth = area.width / (float)chartVisibleBars;
    float barWidth = sectionWidth * 0.75f;

    for (int i = 0; i < count; i++) {
        float open = gameView.price_history[first + i];
        float close = gameView.price_history[first + i + 1];

        float topPrice = (open > close) ? open : close;
        float bottomPrice = (open > close) ? close : open;

        float yTop = PriceToY(topPrice, area, low, span);
        float yBottom = PriceToY(bottomPrice, area, low, span);

        float height = yBottom - yTop;
        if (height < 2.0f) height = 2.0f;

        float x = area.x + i * sectionWidth + (sectionWidth - barWidth) * 0.5f;

        DrawRectangleRec((Rectangle){ x, yTop, barWidth, height }, (close > open) ? GREEN : RED);
    }

    // Display current market price
    float currentY = PriceToY(gameView.current_price, area, low, span);
    DrawLineEx((Vector2){ area.x, currentY },
               (Vector2){ area.x + area.width, currentY }, 2.0f, BLUE);

    const char *priceText = TextFormat("%.0f", gameView.current_price);
    float tagWidth = (float)MeasureText(priceText, chartAxisFontSize) + 12.0f;
    float tagHeight = (float)chartAxisFontSize + 8.0f;
    float tagX = area.x + area.width + 8.0f;
    float tagY = currentY - tagHeight * 0.5f;

    DrawRectangleRec((Rectangle){ tagX, tagY, tagWidth, tagHeight }, BLUE);
    DrawText(priceText, (int)tagX + 6, (int)tagY + 4, chartAxisFontSize, WHITE);
}

static void DrawStatCentered(float x, float width, float y, const char *text, Color color)
{
    DrawText(text, (int)(x + (width - MeasureText(text, fontSize_M)) * 0.5f), (int)y, fontSize_M, color);
}

// Display important stats
static void DrawSidePanel(Rectangle chart)
{
    float top = chart.y + 136.0f;
    float x = chart.x + chart.width + 80.0f;
    float rowHeight = 56.0f;

    float breakEven = (gameView.shares > 0) ? (gameView.starting_cash - gameView.cash) / (float)gameView.shares : 0.0f;

    DrawText(TextFormat("%s%.2f", i18n_T(STR_CURRENT_PRICE), gameView.current_price), (int)x, (int)top, fontSize_L, BLUE);
    DrawText(TextFormat("%s%.2f", i18n_T(STR_BREAK_EVEN), breakEven), (int)x, (int)(top + rowHeight), fontSize_L, DARKGRAY);
    DrawText(TextFormat("%s%d", i18n_T(STR_SHARES), gameView.shares), (int)x, (int)(top + rowHeight * 2.0f), fontSize_L, GRAY);
}

// Display Stats below the chart
static void DrawBottomStats(Rectangle chart)
{
    float y = chart.y + chart.height + 36.0f;
    float colWidth = ((float)screenWidth - 2.0f * chartMargin) / 4.0f;
    float marketValue = (gameView.shares * gameView.current_price < 0.005f) ? 0.0f : gameView.shares * gameView.current_price;

    Color portfolioColor = (gameView.portfolio_value >= gameView.starting_cash) ? DARKGREEN : RED;

    DrawStatCentered(chartMargin, colWidth, y, TextFormat("%s%.2f", i18n_T(STR_PORTFOLIO_VALUE), gameView.portfolio_value), portfolioColor);
    DrawStatCentered(chartMargin + colWidth, colWidth, y, TextFormat("In Market: %.2f", marketValue), DARKGRAY);
    DrawStatCentered(chartMargin + colWidth * 2.0f, colWidth, y, TextFormat("%s%.2f", i18n_T(STR_HIGHEST_VALUE), gameView.highest_ever_price), DARKGRAY);
    DrawStatCentered(chartMargin + colWidth * 3.0f, colWidth, y, TextFormat("%s%.2f", i18n_T(STR_LOWEST_VALUE), gameView.lowest_ever_price), DARKGRAY);
}

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
    
    // Draw Stock Chart
    Rectangle chartArea = GetChartArea();
    DrawChartBars(chartArea);
    DrawRectangleLinesEx(chartArea, 2, BLACK);
    DrawSidePanel(chartArea);
    DrawBottomStats(chartArea);

    // Display remaining time
    const char *time_txt = TextFormat("%s%d:%02d", i18n_T(STR_TIME_REMAINING), (int)gameView.time_remaining / 60, (int)gameView.time_remaining % 60);
    int time_x = (screenWidth - MeasureText(time_txt, fontSize_L)) / 2;
    DrawText(time_txt, time_x, 32, fontSize_L, gameView.time_remaining < 10.0f ? RED : BLACK);

    // Display button control
    int control_y = (screenHeight - fontSize_L - 28);
    DrawText("PRESS to BUY / HOLD to SELL", CenterText("PRESS to BUY / HOLD to SELL", fontSize_L), control_y, fontSize_L, BLACK);

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
