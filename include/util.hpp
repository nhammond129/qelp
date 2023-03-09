#pragma once

#include <SFML/Graphics.hpp>
#include <exception>
#include <imgui.h>
#include <vector>

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
        using duration = std::chrono::duration<double, std::nano>;

        template <typename T, size_t N>
        struct ringbuffer {
            ringbuffer() : size(0), index(0) {}
            void push_back(const T& value) {
                if (size < N) {
                    size++;
                }
                data[index] = value;
                index = (index + 1) % N;
            }
            duration mean() const {
                duration sum = duration::zero();
                for (size_t i = 0; i < size; i++) {
                    sum += data[i];
                }
                return sum / size;
            }
            size_t size;
            size_t index;
            std::vector<T> data = std::vector<T>(N);
        };

    public:
        RAIITimed(const std::string& name) : name(name) {
            start = clock::now();
        }
        ~RAIITimed() {
            auto end = clock::now();
            duration delta = end - start;
            timings[name].push_back(delta);
            delta = timings[name].mean();
            #if 1
            if (oafs[name]) {
                ImGui::Begin("debug");
                ImGui::Columns(2, "timings", true);  // legacy columns api
                ImGui::Text("%s", name.c_str());
                ImGui::NextColumn();
                if (delta <= std::chrono::microseconds(1000)) {
                    ImGui::TextColored(ImVec4{0.f, 1.f, 0.f, 1.f}, "%3d us", std::chrono::duration_cast<microseconds>(delta).count());
                } else if (delta <= std::chrono::milliseconds(2)) {
                    ImGui::TextColored(ImVec4{1.f, 1.f, 0.f, 1.f}, "%3d ms", std::chrono::duration_cast<milliseconds>(delta).count());
                } else if (delta <= std::chrono::seconds(1)) {
                    ImGui::TextColored(ImVec4{1.f, 0.5f, 0.f, 1.f}, "%3d ms", std::chrono::duration_cast<milliseconds>(delta).count());
                } else {
                    ImGui::TextColored(ImVec4{1.f, 0.f, 0.f, 1.f}, "%3d s", std::chrono::duration_cast<seconds>(delta).count());
                }
                ImGui::Columns(1);
                ImGui::End();
            }
            #endif
        }
    private:
        /** N=100 samples **/
        inline static std::unordered_map<std::string, ringbuffer<duration, 100>> timings = {};
        inline static std::unordered_map<std::string, ImGuiOnceUponAFrame> oafs = {};
        std::string name;
        clock::time_point start;
    };

};  // namespace util