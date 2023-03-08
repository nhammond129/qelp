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

namespace bindings {
    static std::unordered_map<input::Action::Type, input::Key> keys = {
        {input::Action::Type::ZoomIn,      input::Key::LBracket},
        {input::Action::Type::ZoomOut,     input::Key::RBracket},
        {input::Action::Type::ZoomReset,   input::Key::Z       },
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

/*
void try_load() noexcept(false); // TODO //
void save(); // TODO //
*/

};  // namespace config
