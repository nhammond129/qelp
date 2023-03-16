#pragma once

#include <SFML/Graphics.hpp>

struct Transformable {
    sf::Transformable& transformable;

    operator sf::Transformable&() { return transformable; }
};