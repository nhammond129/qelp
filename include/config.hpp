#pragma once

#include <string>

namespace config {

static [[maybe_unused]] std::string CONFIG_FILE = "config.ini";
static std::string WINDOW_TITLE = "qelp";
static uint32_t SCREEN_WIDTH = 1600;
static uint32_t SCREEN_HEIGHT = 900;
static uint32_t MAX_FPS = 144;
// TODO
// impl. with SettingsScene ?
// static std::unordered_map<ActionEvent, std::vector<InputEvent>> GAME_KEYBINDS; ?


/*
void try_load() noexcept(false); // TODO //
void save(); // TODO //
*/

};  // namespace config