// Game balance / control configuration — public interface.

#ifndef CONFIG_H
#define CONFIG_H

#include "raylib.h"

// control key bindings
#define CONFIG_KEY_PITCH_UP KEY_SPACE
#define CONFIG_MOUSE_BUTTON_PITCH_UP MOUSE_BUTTON_LEFT

// Returns true while the player is holding the configured pitch-up
// key/mouse binding.
bool Config_IsPitchUpCommanded(void);

#endif // CONFIG_H
