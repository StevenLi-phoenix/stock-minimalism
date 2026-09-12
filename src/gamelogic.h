// Core stock-trading game state and rules — public interface.
// One-button model: short press = BUY (all-in), long hold = SELL (all-out).

#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include "config.h"
#include "utils.h"

#include "raylib.h"

#include <stdbool.h>

typedef enum {
    PLAYER_ACTION_NONE = 0,
    PLAYER_ACTION_BUY,   // short press: released before CONFIG_LONG_HOLD_THRESHOLD
    PLAYER_ACTION_SELL,  // long hold: released at/after CONFIG_LONG_HOLD_THRESHOLD
} PlayerActionType;

typedef struct {
    PlayerActionType type;
    float hold_duration;
} PlayerActionDTO;

typedef struct {
    PlayerActionType type;
    int   price_index;
    float price;
    int   shares;
} PlayerRecordDTO;


#define MAX_PLAYER_RECORDS STOCK_SEQUENCE_LENGTH

typedef struct {
    unsigned int seed;           
    PlayerRecordDTO records[MAX_PLAYER_RECORDS];
    int   record_count;
    float round_seconds;         
} PlayerRecordsDTO;

typedef struct {
    unsigned int seed;
    float price_history[STOCK_SEQUENCE_LENGTH];
    int   price_history_count;
    int   price_index;          // index into price_history for "now"

    float current_price;
    float starting_cash;
    float cash;
    int   shares;
    float portfolio_value;      // cash + shares * current_price
    float profit_loss;          // portfolio_value - starting_cash
    float highest_ever_price;
    float lowest_ever_price;

    bool  holding;               // true while the control key is currently held down
    float hold_duration;         // seconds the control key has been held so far

    float round_seconds;         // total round length, fixed at InitGameState
    float time_remaining;        // seconds left in the round

    bool  failed;                // failed condition: cash + portfolio_value < 0.0f, no more trades allowed, gameover

    PlayerRecordsDTO playback;   // this round's trade log, server/leaderboard payload
} GameViewDTO;

void generate_prices(float *prices, int count);
void InitGameState(GameViewDTO *state, unsigned int seed, float starting_cash, float round_seconds);
PlayerActionDTO ResolvePlayerAction(GameViewDTO *state, bool key_down, float delta_time);
void ApplyPlayerAction(GameViewDTO *state, PlayerActionDTO action);
void UpdateGameState(GameViewDTO *state, float delta_time);
PlayerRecordsDTO BuildPlayerRecords(const GameViewDTO *state);

#endif // GAMELOGIC_H
