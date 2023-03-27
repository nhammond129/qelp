#pragma once

#include <cstdint>
#include <input/input.hpp>
#include <string>
#include <unordered_map>

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

namespace bindings {
    static input::Modifier ActionQueueModifier = input::Key::LShift;
    static std::unordered_map<input::Action::Type, input::Key> keys = {
        {input::Action::Type::ZoomIn,      input::Key::LBracket},
        {input::Action::Type::ZoomOut,     input::Key::RBracket},
        {input::Action::Type::ZoomReset,   input::Key::Z       },
        {input::Action::Type::ViewCenter,  input::Key::Space   }
    };
    static std::unordered_map<input::Action::Type, input::MouseButton> mousebuttons = {
        {input::Action::Type::MoveClick, input::MouseButton::Right},
    };
    static std::unordered_map<input::Action::Type, input::MouseWheel>  mousewheels = {
        {input::Action::Type::ZoomIn,  input::MouseWheel::VerticalWheel}
    };
    static std::unordered_map<input::Action::Type, input::JoyAxis>     joyaxes;
    static std::unordered_map<input::Action::Type, input::JoyButton>   joybuttons;
};  // namespace bindings

};  // namespace config
