// Core stock-trading game state and rules.
// See gamelogic.h for the public interface.

#include "gamelogic.h"

// maybe we should use a more sophisticated price generator, current is just random walk
// and btw it need to be deterministic based on the seed
// TODO: fix this to use a more realistic price generator, e.g., geometric Brownian motion
void generate_prices(float *prices, int count)
{
    if (count <= 0) return;

    prices[0] = 100.0f;
    for (int i = 1; i < count; i++) {
        float change = GetRandomValue(-100, 100);
        prices[i] = prices[i - 1] + change;
        // if (prices[i] < -10.0f) prices[i] = -10.0f; // -10 floor; price can go negative (see failed/bankruptcy check)
    }
}

void InitGameState(GameViewDTO *state, unsigned int seed, float starting_cash, float round_seconds)
{
    *state = (GameViewDTO){ 0 };

    state->seed = (seed != 0) ? seed : (unsigned int)gameSeedGenerator(0);
    SetRandomSeed(state->seed);

    generate_prices(state->price_history, STOCK_SEQUENCE_LENGTH);
    state->price_history_count = STOCK_SEQUENCE_LENGTH;
    state->price_index = 0;
    state->current_price = state->price_history[0];

    state->starting_cash = starting_cash;
    state->cash = starting_cash;
    state->shares = 0;
    state->portfolio_value = starting_cash;
    state->profit_loss = 0.0f;
    state->highest_ever_price = state->current_price;
    state->lowest_ever_price = state->current_price;

    state->holding = false;
    state->hold_duration = 0.0f;

    state->round_seconds = round_seconds;
    state->time_remaining = round_seconds;

    state->playback.seed = state->seed;
    state->playback.round_seconds = round_seconds;
    state->playback.record_count = 0;

    TraceLog(LOG_INFO, "InitGameState: seed=%u cash=%.2f round=%.1fs ticks=%d",
             state->seed, starting_cash, round_seconds, STOCK_SEQUENCE_LENGTH);
}


static void RecordPlayerAction(GameViewDTO *state, PlayerActionType type, int shares)
{
    PlayerRecordsDTO *playback = &state->playback;
    if (playback->record_count >= MAX_PLAYER_RECORDS) {
        TraceLog(LOG_WARNING, "RecordPlayerAction: playback log full, dropping record");
        return;
    }

    PlayerRecordDTO *record = &playback->records[playback->record_count++];
    record->type = type;
    record->price_index = state->price_index;
    record->price = state->current_price;
    record->shares = shares;
}

PlayerActionDTO ResolvePlayerAction(GameViewDTO *state, bool key_down, float delta_time)
{
    PlayerActionDTO action = { PLAYER_ACTION_NONE, 0.0f };

    if (key_down) {
        state->holding = true;
        state->hold_duration += delta_time;
        return action; // still holding: action only fires on release
    }

    if (state->holding) {
        // key was released this frame: classify short press vs. long hold
        action.hold_duration = state->hold_duration;
        action.type = (state->hold_duration >= CONFIG_LONG_HOLD_THRESHOLD)
            ? PLAYER_ACTION_SELL
            : PLAYER_ACTION_BUY;

        TraceLog(LOG_DEBUG, "ResolvePlayerAction: released after %.2fs -> %s",
                 state->hold_duration,
                 action.type == PLAYER_ACTION_SELL ? "SELL" : "BUY");
    }

    state->holding = false;
    state->hold_duration = 0.0f;
    return action;
}

void ApplyPlayerAction(GameViewDTO *state, PlayerActionDTO action)
{
    if (state->failed) return; // bankrupt: no more trades this round

    switch (action.type) {
        case PLAYER_ACTION_BUY: {
            if (state->current_price <= 0.0f || state->cash < state->current_price) break;

            int shares_bought = CONFIG_SHARES_PER_TRADE;
            state->shares += shares_bought;
            state->cash -= shares_bought * state->current_price;
            RecordPlayerAction(state, PLAYER_ACTION_BUY, shares_bought);
            TraceLog(LOG_INFO, "ApplyPlayerAction: BUY %d @ %.2f, cash left %.2f",
                     shares_bought, state->current_price, state->cash);
            break;
        }
        case PLAYER_ACTION_SELL: {
            if (state->shares <= 0) break;

            int shares_sold = CONFIG_SHARES_PER_TRADE;
            if (shares_sold > state->shares) shares_sold = state->shares;

            float proceeds = shares_sold * state->current_price;
            TraceLog(LOG_INFO, "ApplyPlayerAction: SELL %d @ %.2f for %.2f",
                     shares_sold, state->current_price, proceeds);

            state->cash += proceeds;
            state->shares -= shares_sold;
            RecordPlayerAction(state, PLAYER_ACTION_SELL, shares_sold);
            break;
        }
        case PLAYER_ACTION_NONE:
        default:
            break;
    }
}

void UpdateGameState(GameViewDTO *state, float delta_time)
{
    if (state->failed) return; // game over: freeze the clock/price/portfolio where they stand

    state->time_remaining -= delta_time;
    if (state->time_remaining < 0.0f) state->time_remaining = 0.0f;

    if (state->round_seconds > 0.0f && state->price_history_count > 1) {
        float elapsed = state->round_seconds - state->time_remaining;
        float progress = elapsed / state->round_seconds;
        if (progress < 0.0f) progress = 0.0f;
        if (progress > 1.0f) progress = 1.0f;

        int index = (int)(progress * (state->price_history_count - 1));
        if (index >= state->price_history_count) index = state->price_history_count - 1;

        state->price_index = index;
        state->current_price = state->price_history[index];
    }

    if (state->current_price > state->highest_ever_price) state->highest_ever_price = state->current_price;
    if (state->current_price < state->lowest_ever_price) state->lowest_ever_price = state->current_price;

    state->portfolio_value = state->cash + state->shares * state->current_price;
    state->profit_loss = state->portfolio_value - state->starting_cash;

    if (!state->failed && (state->cash + state->portfolio_value) < 0.0f) {
        state->failed = true;
        TraceLog(LOG_WARNING, "UpdateGameState: player failed (cash=%.2f portfolio=%.2f)",
                 state->cash, state->portfolio_value);
    }
}

PlayerRecordsDTO BuildPlayerRecords(const GameViewDTO *state)
{
    return state->playback;
}
