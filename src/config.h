// Game balance / control configuration — public interface.

#ifndef CONFIG_H
#define CONFIG_H

#include "raylib.h"

// control key bindings
#define CONFIG_PRIMARY_KEY_CONTROL KEY_SPACE
#define CONFIG_ALT_KEY_CONTROL MOUSE_BUTTON_LEFT

// random seed generator configuration
#define STOCK_SEQUENCE_LENGTH 300

// round / balance configuration
#define CONFIG_STARTING_CASH 1000.0f

// total round length in seconds (240s = 4min). The STOCK_SEQUENCE_LENGTH
// price ticks are spread evenly across this whole duration.
#define CONFIG_ROUND_SECONDS 240.0f

// shares traded per BUY/SELL: one press moves one unit of the order stack,
// not an all-in/all-out swing (short press and long hold both trade this many).
#define CONFIG_SHARES_PER_TRADE 1

// one-button input: release before this many seconds held = BUY (short press),
// release at/after it = SELL (long hold). See README.md for the concept.
#define CONFIG_LONG_HOLD_THRESHOLD 0.2f

// Returns true while the player is holding the configured control key
bool IsControlKeyPressed(void);

#endif // CONFIG_H
