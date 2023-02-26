#include <util.hpp>
#include <SFML/Graphics.hpp>
#include <exception>

namespace util {

sf::Texture programmerArtTexture(float width, float height, const sf::Color color, const sf::Color background) {
    sf::RenderTexture rt;
    if (!rt.create({(unsigned int)width, (unsigned int)height})) throw std::runtime_error("Error creating render texture");

    // colored border
    sf::RectangleShape rect({width-2, height-2});
    rect.setOutlineColor(color);
    rect.setOutlineThickness(1);
    rect.setFillColor(background);
    rect.setPosition({1, 1});
    rt.draw(rect);

    // draw arrow in the 'middle', pointing 'right' from x=0 to x=width
    sf::VertexArray lines(sf::PrimitiveType::Lines, 6);
    {
        lines[0].position = {width * 0.1f, height / 2.f};
        lines[1].position = {width * 0.9f, height / 2.f};
        lines[2].position = {width * 0.9f, height / 2.f};
        lines[3].position = {width * 0.8f, height / 2.f - height * 0.1f};
        lines[4].position = {width * 0.9f, height / 2.f};
        lines[5].position = {width * 0.8f, height / 2.f + height * 0.1f};
    }
    for (int i = 0; i < 6; ++i) { lines[i].color = color; }
    rt.draw(lines);

    rt.display();

    // explicit clone
    return sf::Texture {rt.getTexture()};
}

};  // namespace util