#pragma once

#include <cstdint>
#include <string>
#include <SFML/System.hpp>

/**
 * @brief global configuration
 * 
 * TODO: impl. save/load/edit functionality for this
 **/
namespace config {

// static std::string CONFIG_FILE = "config.ini"; /** UNUSED **/
static std::string WINDOW_TITLE = "qelp";
static uint32_t SCREEN_WIDTH = 1600;
static uint32_t SCREEN_HEIGHT = 900;
static uint32_t MAX_FPS = 144;
static sf::Time FIXED_UPDATE_INTERVAL = sf::seconds(1.f / 60.f);
static uint16_t SERVER_PORT = 63042;


};  // namespace config
