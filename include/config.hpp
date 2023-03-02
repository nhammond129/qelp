#pragma once

#include <string>

/**
 * @brief global configuration
 * 
 * TODO: impl. save/load/edit functionality for this
 **/
namespace config {

static std::string CONFIG_FILE = "config.ini";
static std::string WINDOW_TITLE = "qelp";
static uint32_t SCREEN_WIDTH = 1600;
static uint32_t SCREEN_HEIGHT = 900;
static uint32_t MAX_FPS = 144;
// TODO
// impl. with SettingsScene ?
// static std::unordered_map<InputEvent, std::vector<Action>> GAME_KEYBINDS; ?


/*
void try_load() noexcept(false); // TODO //
void save(); // TODO //
*/

};  // namespace config
