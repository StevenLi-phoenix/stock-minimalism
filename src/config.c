// included balance_config
// initial deorbit speed: 8000 m/s
// initial altitude: 120000 m
// initial angle: 90 degrees
// target landing speed: ?
// target landing altitude: 0 m
// stall speed
// pitch
// trim mappings

#include "config.h"

// control key bindings: CONFIG_KEY_PITCH_UP || CONFIG_MOUSE_BUTTON_PITCH_UP
bool Config_IsPitchUpCommanded(void)
{
    return IsKeyDown(CONFIG_KEY_PITCH_UP) || IsMouseButtonDown(CONFIG_MOUSE_BUTTON_PITCH_UP);
}