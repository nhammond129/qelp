#pragma once

#include <SFML/Graphics.hpp>

struct Drawable {
    sf::Drawable& drawable;
    size_t render_index = 0;  // for depth sorting

    operator sf::Drawable&() { return drawable; }
};