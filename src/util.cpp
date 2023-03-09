#include <util.hpp>
#include <SFML/Graphics.hpp>
#include <exception>

namespace util {

sf::Texture& programmerArtTexture(float width, float height, const sf::Color color, const sf::Color background, const std::vector<sf::Vector2f> &points) {
    static std::forward_list<sf::Texture> static_textures;  // static so that the textures are not destroyed when the function returns
                                                            // This is dangerous, but it's okay as a temporary measure until I can figure out a better way to do this
                                                            // TODO: remove. This is just asking for wasted memory.
    sf::RenderTexture rt;
    if (!rt.create({(unsigned int)width, (unsigned int)height})) throw std::runtime_error("Error creating render texture");

    // colored border
    sf::RectangleShape rect({width-2, height-2});
    rect.setOutlineColor(color);
    rect.setOutlineThickness(1);
    rect.setFillColor(background);
    rect.setPosition({1, 1});
    rt.draw(rect);

    {   // draw arrow in the 'middle', pointing 'right' from x=0 to x=width
        sf::VertexArray lines(sf::PrimitiveType::Lines, 6);
        lines.append({{width * 0.1f, height / 2.f}, color});
        lines.append({{width * 0.9f, height / 2.f}, color});
        lines.append({{width * 0.9f, height / 2.f}, color});
        lines.append({{width * 0.8f, height / 2.f - height * 0.1f}, color});
        lines.append({{width * 0.9f, height / 2.f}, color});
        lines.append({{width * 0.8f, height / 2.f + height * 0.1f}, color});
        rt.draw(lines);
    }

    {   // mark a little yellow x on each point
        sf::VertexArray lines(sf::PrimitiveType::Lines, points.size() * 4);
        for (const auto& p : points) {
            lines.append({{p.x - 2, p.y - 2}, sf::Color::Yellow});
            lines.append({{p.x + 2, p.y + 2}, sf::Color::Yellow});
            lines.append({{p.x - 2, p.y + 2}, sf::Color::Yellow});
            lines.append({{p.x + 2, p.y - 2}, sf::Color::Yellow});
        }
        rt.draw(lines);
    }

    rt.display();
    static_textures.emplace_front(rt.getTexture());

    return static_textures.front();
}

};  // namespace util