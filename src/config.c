#include "config.h"

// control key bindings: CONFIG_PRIMARY_KEY_CONTROL || CONFIG_ALT_KEY_CONTROL
bool IsControlKeyPressed(void)
{
    return IsKeyDown(CONFIG_PRIMARY_KEY_CONTROL) || IsMouseButtonDown(CONFIG_ALT_KEY_CONTROL);
}

