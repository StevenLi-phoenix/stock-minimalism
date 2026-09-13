# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/).

## [Unreleased]

## [0.0.3] - 2026-09-12

### Added
- Game UI built out in `main.c`, with new i18n strings.
  Files: src/main.c, src/i18n.c, src/i18n.h

### Fixed
- Fixed incorrect stat name in i18n string table.
  Files: src/i18n.c

## [0.0.2] - 2026-09-12

### Known Issues
- i18n: LANG_ZH not working due to default font not supporting Chinese characters.

### Added
- Core trading Game logic, expose via GameViewDTO struct and UpdateGameState() function.
- `scripts/build_native.sh` / `run_local.sh`: native desktop build script.
- Debug HUD in `main.c` 

### Changed
- Renamed project from `reentry-minimalism` to `stock-minimalism` 
- Game concept changed from "space shuttle reentry" to "stock trading simulator" 

## [0.0.1] - 2026-09-12 — Initial Prototype

### Added
- Project scaffold: `src/`, `scripts/`, `build/web/` (gitignored) layout.
- CI added to github push actions. Auto-release to itch.io via butler on push to `main`.
- Added a README.md
- Initial a simple game loop in main.c.
- Added runway texture asset (RunWay.png).
  Files: assets/RunWay.png
- Added i18n string-table support (EN/ZH).
  Files: src/i18n.h, src/i18n.c
- Added config.c and gamelogic.c placeholders for upcoming balance/physics logic.
  Files: src/config.c, src/gamelogic.c

### Changed
- Updated CREDITS.md to link asset filenames directly and credit the runway texture source.
  Files: assets/CREDITS.md
- Bumped resolution to 1280x720, split main loop into HandleInput/UpdateGameState/DrawFrame, and added shuttle texture with pitch rotation.
  Files: src/main.c
- build_web.sh now compiles all src/*.c files instead of just main.c.
  Files: scripts/build_web.sh
- CI caches the raylib web build by submodule commit sha.
  Files: .github/workflows/itch-release.yml

