#pragma once

#include <entt/entt.hpp>
#include <exception>
#include <imgui.h>
#include <SFML/Graphics.hpp>
#include <vector>

/**
 * general utility and miscellaneous functions
 **/
namespace util {

    /**
     * @brief Helper for adding interface component dependencies
     * @note This passes the instance of Parent to the constructor of Dependent
     **/
    template <typename Parent, typename Dependent>
    void add_dependent_iface(entt::registry& registry) {
        registry.on_construct<Parent>().connect<
            [](entt::registry& registry, entt::entity entity) {
                registry.emplace<Dependent>(entity, registry.get<Parent>(entity));
            }>();
    }

    /**
     * @brief Helper for adding generic component dependencies
     **/
    template <typename Parent, typename Dependent>
    void add_dependent(entt::registry& registry) {
        registry.on_construct<Parent>().connect<
            [](entt::registry& registry, entt::entity entity) {
                registry.emplace<Dependent>(entity);
            }>();
    }

    /**
     * @brief Check intersection of two sf::Rect<T> AABBs
     **/
    template <typename T>
    bool intersects(const sf::Rect<T>& a, const sf::Rect<T>& b);

    /**
     * @brief programmer art texture generation
     * @param width  The width of the texture
     * @param height The height of the texture
     * @param color  The color of the texture
     * @param background The background color of the texture
     * @param points The points to mark on the texture
     * 
     * Generates a texture with a given width, height, and color.
     * Has an arrow pointing to the "right" (positive x-axis)
     **/
    sf::Texture& programmerArtTexture(
        float width, float height, const sf::Color color,
        const sf::Color background = sf::Color::Transparent,
        const std::vector<sf::Vector2f>& points = {}
    );

    // As cursed as it would be, I kind of wish I could inject this as a member function of sf::RenderTarget.
    // Because I wanna.
    void setViewCenter(sf::RenderTarget& target, const sf::Vector2f& center);
    void moveViewCenter(sf::RenderTarget& target, const sf::Vector2f& offset);

};  // namespace util