// i18n localization support
//
// Minimal string-table based i18n. See i18n.h for the public interface.

#include <stddef.h>

#include "i18n.h"
#include "raylib.h"

// Table is [language][key]. Keep the row order in sync with StringKey,
static const char *i18n_strings[LANG_COUNT][STR_COUNT] = {
    [LANG_EN] = {
        [STR_GAME_TITLE] = "Reentry Minimalism",
        [STR_PRESS_SPACE_TO_START] = "Press SPACE to start",
        [STR_PAUSED] = "Paused",
        [STR_GAME_OVER] = "Game Over",
        [STR_PIPELINE_OK] = "reentry-minimalism: raylib + WebAssembly pipeline OK",
    },
    [LANG_ZH] = {
        [STR_GAME_TITLE] = "再入极简",
        [STR_PRESS_SPACE_TO_START] = "按空格键开始",
        [STR_PAUSED] = "已暂停",
        [STR_GAME_OVER] = "游戏结束",
        [STR_PIPELINE_OK] = "再入极简：raylib + WebAssembly 构建流程正常",
    },
};

static Language i18n_currentLanguage = LANG_EN;

void i18n_SetLanguage(Language language)
{
    if (language < 0 || language >= LANG_COUNT)
    {
        TraceLog(LOG_WARNING, "i18n: unknown language %d, falling back to LANG_EN", language);
        language = LANG_EN;
    }
    i18n_currentLanguage = language;
}

Language i18n_GetLanguage(void)
{
    return i18n_currentLanguage;
}

const char *i18n_T(StringKey key)
{
    if (key < 0 || key >= STR_COUNT)
    {
        return "???";
    }

    const char *text = i18n_strings[i18n_currentLanguage][key];
    if (text != NULL)
    {
        return text;
    }

    text = i18n_strings[LANG_EN][key];
    if (text != NULL)
    {
        return text;
    }

    TraceLog(LOG_WARNING, "i18n: missing translation for key %d", key);
    return "MISSING_STRING";
}
