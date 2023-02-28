#pragma once

#include <SFML/Graphics.hpp>

namespace scenes {

/**
 * @brief Interface for all scenes
 * 
 * Provides a common interface for all scenes.
 * Modelled after Andres6936's IScreen for Cataclysm
 * https://github.com/Andres6936/Cataclysm/blob/master/Include/Cataclysm/Screen/IScreen.hpp
 */
class IScene {
protected:
    sf::Clock mClock;
    sf::RenderWindow* mWindowptr = nullptr;
public:
    /* Abstract interface functions */
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual void update(const sf::Time& dt) = 0;
    virtual void handleEvent(const sf::Event& event) = 0;
};

};  // namespace scenes