#pragma once

#include <SFML/Graphics.hpp>

namespace scenes {

/**
 * @brief Interface for all scenes
 * 
 * Provides a common interface for all scenes.
 * Modelled after Andres6936's IScreen for Cataclysm
 * https://github.com/Andres6936/Cataclysm/blob/master/Include/Cataclysm/Screen/IScreen.hpp
 */
class IScene {
private:
    static sf::Clock mImguiClock;
protected:
    static sf::Clock mClock;
    static sf::RenderWindow mWindow;

    /* imgui input & drawing */
    static void handleEvent_imgui(const sf::Event& event);
    static void draw_imgui();  // NOTE: Call this in `draw()` of derived classes if you want imgui to display !!
public:
    /* functions for managing imgui from `SceneManager` */
    static void init_imgui();
    static void update_imgui();
    static void shutdown_imgui();

    /* Abstract interface functions */
    virtual void draw() = 0;
    virtual void update() = 0;
    virtual void handleEvents() = 0;
};

};  // namespace scenes