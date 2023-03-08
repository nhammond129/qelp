#pragma once

#include <optional>
#include <SFML/Window.hpp>

namespace input {

using Key = sf::Keyboard::Key;
using MouseButton = sf::Mouse::Button;
using MouseWheel = sf::Mouse::Wheel;
using JoyButton = uint32_t;
using JoyAxis = sf::Joystick::Axis;

struct Action {
    enum class Type {
        ZoomIn,
        ZoomOut,
        ZoomReset,
        MoveClick,
    };

    struct Zoom {
        float amount = 0.f;
    };

    struct MoveClick {
        sf::Vector2i pos;
    };

    Type type;
    union {
        Zoom zoom;
        MoveClick move;
    };
};

std::optional<Action> getAction(const sf::Event& event);

};  // namespace input