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

    // Timing
    class RAIITimed {
        using clock = std::chrono::high_resolution_clock;
        using microseconds = std::chrono::microseconds;
        using milliseconds = std::chrono::milliseconds;
        using seconds = std::chrono::seconds;
    public:
        RAIITimed(const std::string& name) : name(name) {
            start = clock::now();
        }
        ~RAIITimed() {
            auto end = clock::now();
            std::chrono::duration<double, std::nano> delta = end - start;
            #if 1
            ImGui::Begin("debug");
            if (delta <= std::chrono::microseconds(1000)) {
                ImGui::Text("%s: %3d us", name.c_str(), std::chrono::duration_cast<microseconds>(delta).count());
            } else if (delta <= std::chrono::milliseconds(1000)) {
                ImGui::Text("%s: %3d ms", name.c_str(), std::chrono::duration_cast<milliseconds>(delta).count());
            } else {
                ImGui::Text("%s:  %3d s",  name.c_str(), std::chrono::duration_cast<seconds>(delta).count());
            }
            ImGui::End();
            #endif
        }
    private:
        std::string name;
        clock::time_point start;
    };

};  // namespace util