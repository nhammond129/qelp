#include <exception>
#include <forward_list>
#include <iostream>
#include <mutex>
#include <SFML/Graphics.hpp>
#include <util.hpp>

namespace util {

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
