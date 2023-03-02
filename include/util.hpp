#pragma once

#include <SFML/Graphics.hpp>
#include <exception>

/**
 * general utility and miscellaneous functions
 **/
namespace util {

    /**
     * @brief programmer art texture generation
     * @param width  The width of the texture
     * @param height The height of the texture
     * @param color  The color of the texture
     * @param background The background color of the texture
     * 
     * Generates a texture with a given width, height, and color.
     * Has an arrow pointing to the "right" (positive x-axis)
     **/
    sf::Texture programmerArtTexture(float width, float height, sf::Color color, sf::Color background = sf::Color::Transparent);

};  // namespace util