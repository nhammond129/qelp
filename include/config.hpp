#pragma once

#include <string>

namespace config {

static [[maybe_unused]] std::string CONFIG_FILE = "config.ini";
static std::string WINDOW_TITLE = "qelp";
static uint32_t SCREEN_WIDTH = 1600;
static uint32_t SCREEN_HEIGHT = 900;
static uint32_t MAX_FPS = 144;

void loadConfig(); // TODO //
void saveConfig(); // TODO //

};  // namespace config