// i18n localization support — public interface.
//
// Usage:
//   i18n_SetLanguage(LANG_ZH);
//   DrawText(i18n_T(STR_GAME_TITLE), 24, 24, 20, DARKGRAY);

#ifndef I18N_H
#define I18N_H

typedef enum
{
    LANG_EN = 0,
    LANG_ZH,
    LANG_COUNT
} Language;

typedef enum
{
    STR_GAME_TITLE = 0,
    STR_PRESS_SPACE_TO_START,
    STR_PAUSED,
    STR_GAME_OVER,
    STR_PIPELINE_OK,
    STR_COUNT
} StringKey;

// Switches the active language. Falls back to LANG_EN on an out-of-range value.
void i18n_SetLanguage(Language language);

Language i18n_GetLanguage(void);

// Looks up a string in the current language. Falls back to LANG_EN, then to
// a placeholder, so a missing translation never returns NULL.
const char *i18n_T(StringKey key);

#endif // I18N_H
