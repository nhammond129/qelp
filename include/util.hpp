#pragma once

#include <SFML/Graphics.hpp>
#include <exception>

/*
    general utility and miscellaneous functions
*/
namespace util {

    sf::Texture programmerArtTexture(float width, float height, sf::Color color, sf::Color background = sf::Color::Transparent);

};  // namespace util