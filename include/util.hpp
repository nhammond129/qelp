#pragma once

#include <chrono>
#include <cstddef>
#include <entt/entt.hpp>
#include <exception>
#include <SFML/Graphics.hpp>
#include <source_location>
#include <vector>

/**
 * general utility and miscellaneous functions
 **/
namespace util {

    using Clock = std::chrono::high_resolution_clock;
    using Time = std::chrono::time_point<Clock>;
    using Duration = std::chrono::duration<float>;

    /**
     * @brief logging helper
     * @note This will always log, even in release builds
     * @see util::debuglog
     **/
    void log(const std::string& message, const std::source_location& location = std::source_location::current());

    /**
     * @brief logging helper that only logs during debug builds
     * @note This is a no-op in release builds
     * @see util::log
     **/
    void debuglog(const std::string& message, const std::source_location& location = std::source_location::current());

    /**
     * @brief Check intersection of two sf::Rect<T> AABBs
     **/
    template <typename T>
    bool intersects(const sf::Rect<T>& a, const sf::Rect<T>& b);

    // As cursed as it would be, I kind of wish I could inject this as a member function of sf::RenderTarget.
    // Because I wanna.
    void setViewCenter(sf::RenderTarget& target, const sf::Vector2f& center);
    void moveViewCenter(sf::RenderTarget& target, const sf::Vector2f& offset);

};  // namespace util