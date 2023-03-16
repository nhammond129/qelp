#pragma once

#include <SFML/Graphics.hpp>

class Drawable {
public:
    Drawable(sf::Drawable& sfdrawable): sfdrawable(sfdrawable) {}
    operator sf::Drawable&() const { return sfdrawable; }

    sf::Drawable& sfdrawable;
    int16_t render_index = 0;  // for depth sorting
};