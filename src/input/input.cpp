#include <config.hpp>
#include <input/input.hpp>

namespace input {

bool isModifierPressed(const Modifier& mod, const unsigned int joyid) {
    return std::visit([joyid](const auto& m) -> bool {
        using T = std::decay_t<decltype(m)>;
        if constexpr (std::is_same_v<T, Key>) {
            return sf::Keyboard::isKeyPressed(m);
        } else if constexpr (std::is_same_v<T, MouseButton>) {
            return sf::Mouse::isButtonPressed(m);
        } else if constexpr (std::is_same_v<T, JoyButton>) {
            return sf::Joystick::isButtonPressed(joyid, m);
        }
    }, mod);
}

std::optional<Action> getAction(const sf::Event& event) {
    /**
     * This function is a bit of a mess, the crux of the issue lies
     * in the sf::Event union and that we have N actions and M events,
     * with each event needing slightly different handling for a given action.
     **/
    switch (event.type) {
        case sf::Event::MouseWheelScrolled: {
            for (const auto& [actiont, wheel] : config::bindings::mousewheels) {
                if (event.mouseWheelScroll.wheel == wheel) {
                    Action action = { .type = actiont };
                    switch (action.type) {
                        case Action::Type::ZoomIn:
                            action.zoom.amount = event.mouseWheelScroll.delta;
                            break;
                        case Action::Type::ZoomOut:
                            action.zoom.amount = -event.mouseWheelScroll.delta;
                            break;
                        case Action::Type::ZoomReset:
                            action.zoom.amount = 0.f;
                            break;
                        case Action::Type::MoveClick:
                            action.move = {
                                .pos = {event.mouseWheelScroll.x, event.mouseWheelScroll.y},
                                .queue = isModifierPressed(config::bindings::ActionQueueModifier)
                            };
                            break;
                        case Action::Type::ViewCenter:
                            break;
                    }
                    return action;
                }
            }
        }
        case sf::Event::MouseButtonPressed: {
            for (const auto& [actiont, button] : config::bindings::mousebuttons) {
                if (event.mouseButton.button == button) {
                    Action action = { .type = actiont };
                    switch (action.type) {
                        case Action::Type::ZoomIn:
                            action.zoom.amount = 1.f;
                        case Action::Type::ZoomOut:
                            break;
                        case Action::Type::ZoomReset:
                            action.zoom.amount = 0.f;
                            break;
                        case Action::Type::MoveClick:
                            action.move = {
                                .pos = {event.mouseButton.x, event.mouseButton.y},
                                .queue = isModifierPressed(config::bindings::ActionQueueModifier)
                            };
                            break;
                        case Action::Type::ViewCenter:
                            break;
                    }
                    return action;
                }
            }
            break;
        }
        case sf::Event::KeyPressed: {
            for (const auto& [actiont, key] : config::bindings::keys) {
                if (event.key.code == key) {
                    Action action = { .type = actiont };
                    switch (action.type) {
                        case Action::Type::ZoomIn:
                            action.zoom.amount = 1.f;
                        case Action::Type::ZoomOut:
                            break;
                        case Action::Type::ZoomReset:
                            action.zoom.amount = 0.f;
                            break;
                        case Action::Type::MoveClick:
                            action.move = {
                                .pos = {sf::Mouse::getPosition().x, sf::Mouse::getPosition().y},
                                .queue = isModifierPressed(config::bindings::ActionQueueModifier)
                            };
                            break;
                        case Action::Type::ViewCenter:
                            break;
                    }
                    return action;
                }
            }
            break;
        }
        case sf::Event::JoystickButtonPressed: {
            for (const auto& [actiont, button] : config::bindings::joybuttons) {
                if (event.joystickButton.button == button) {
                    Action action = { .type = actiont };
                    switch (action.type) {
                        case Action::Type::ZoomIn:
                            action.zoom.amount = 1.f;
                        case Action::Type::ZoomOut:
                            break;
                        case Action::Type::ZoomReset:
                            action.zoom.amount = 0.f;
                            break;
                        case Action::Type::MoveClick:
                            action.move = {
                                .pos = {sf::Mouse::getPosition().x, sf::Mouse::getPosition().y},
                                .queue = isModifierPressed(config::bindings::ActionQueueModifier)
                            };
                            break;
                        case Action::Type::ViewCenter:
                            break;
                    }
                    return action;
                }
            }
            break;
        }
        case sf::Event::JoystickMoved: {
            for (const auto& [actiont, axis] : config::bindings::joyaxes) {
                if (event.joystickMove.axis == axis) {
                    Action action = { .type = actiont };
                    switch (action.type) {
                        case Action::Type::ZoomIn:
                            action.zoom.amount = event.joystickMove.position;
                        case Action::Type::ZoomOut:
                            break;
                        case Action::Type::ZoomReset:
                            action.zoom.amount = 0.f;
                            break;
                        case Action::Type::MoveClick:
                            // defaulting to mouse position
                            action.move = {
                                .pos = {sf::Mouse::getPosition().x, sf::Mouse::getPosition().y},
                                .queue = isModifierPressed(config::bindings::ActionQueueModifier)
                            };
                            break;
                        case Action::Type::ViewCenter:
                            break;
                    }
                    return action;
                }
            }
            break;
        }
    }
    return std::nullopt;
}

};  // namespace input