#pragma once

#include <SFML/Graphics.hpp>

namespace scenes {

class IScene {
public:
    virtual void draw(sf::RenderWindow&) = 0;
    virtual void update() = 0;
    virtual void handleEvents(sf::RenderWindow&) = 0;
};

};  // namespace scenes