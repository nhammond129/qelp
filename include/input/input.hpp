#pragma once

#include <optional>
#include <SFML/Window.hpp>
#include <variant>

namespace input {

using Key = sf::Keyboard::Key;
using MouseButton = sf::Mouse::Button;
using MouseWheel = sf::Mouse::Wheel;
using JoyButton = uint32_t;
using JoyAxis = sf::Joystick::Axis;
using Modifier = std::variant<Key, MouseButton, JoyButton>;

bool isModifierPressed(const Modifier& mod, const unsigned int joyid = 0);

struct Action {
    enum class Type {
        ZoomIn,
        ZoomOut,
        ZoomReset,
        ViewCenter,
        MoveClick,
    };

    struct Zoom {
        float amount = 0.f;
    };

    struct MoveClick {
        sf::Vector2i pos;
        bool queue = false;
    };

    Type type;
    union {
        Zoom zoom;
        MoveClick move;
    };
};

std::optional<Action> getAction(const sf::Event& event);

};  // namespace input