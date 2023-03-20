#include <exception>
#include <forward_list>
#include <iostream>
#include <mutex>
#include <SFML/Graphics.hpp>
#include <util.hpp>

namespace util {

/**
 * FUTURE: (probably by april '23) use `std::format`
 *
 * If std::format was available for gcc < 13, I would use it here to provide an easier variadic interface.
 * Alas, since gcc 13 is not officially released yet, I will have to be satisfied with this for now.
 **/
void log(const std::string& msg, const std::source_location& loc) {
    static std::mutex sIOlock;
    {
        std::lock_guard<std::mutex> lock(sIOlock);
        std::cout
        #ifndef NDEBUG
            << "[" << loc.file_name() << ":" << loc.line() << "] "
        #endif
            << msg
            << std::endl;
    }
}

void debuglog(const std::string& msg, const std::source_location& loc) {
    #ifndef NDEBUG
        log(msg, loc);
    #endif
}

template <typename T>
bool intersects(const sf::Rect<T>& a, const sf::Rect<T>& b)  {
    return (
        (a.top < b.top + b.height && a.top + a.height > b.top) &&
        (a.left < b.left + b.width && a.left + a.width > b.left)
    );
}

sf::Texture& programmerArtTexture(float width, float height, const sf::Color color, const sf::Color background, const std::vector<sf::Vector2f> &points) {
    static std::forward_list<sf::Texture> static_textures;  // static so that the textures are not destroyed when the function returns
                                                            // This is dangerous, but it's okay as a temporary measure until I can figure out a better way to do this
                                                            // TODO: remove. This is just asking for wasted memory.
    sf::RenderTexture rt;
    if (!rt.create({(unsigned int)width, (unsigned int)height})) throw std::runtime_error("Error creating render texture");

    // colored border
    constexpr uint8_t border = 2;
    sf::RectangleShape rect({width-(2*border), height-(2*border)});
    rect.setOutlineColor(color);
    rect.setOutlineThickness(border);
    rect.setFillColor(background);
    rect.setPosition({border, border});
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

void setViewCenter(sf::RenderTarget& target, const sf::Vector2f& center) {
    auto view = target.getView();
    view.setCenter(center);
    target.setView(view);
}

void moveViewCenter(sf::RenderTarget& target, const sf::Vector2f& offset) {
    auto view = target.getView();
    view.move(offset);
    target.setView(view);
}

};  // namespace util
