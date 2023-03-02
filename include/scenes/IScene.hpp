#pragma once

#include <SFML/Graphics.hpp>

namespace scenes {

class SceneManager;

/**
 * @brief Interface for all scenes
 * 
 * Provides a common interface for all scenes.
 * Modelled after Andres6936's IScreen for Cataclysm
 * https://github.com/Andres6936/Cataclysm/blob/master/Include/Cataclysm/Screen/IScreen.hpp
 */
class IScene {
protected:
    sf::Clock mClock;       /** Unused? */
    SceneManager& mManager; /** Reference to the parent scene manager */

public:
    IScene() = delete;               /** no default constructor **/
    IScene(const IScene&) = delete;  /** no copy-constructor    **/

    /**
     * @brief Constructor.
     * @param manager The parent scene manager
     * 
     * Constructs an IScene with a reference to the parent scene manager.
     * Exists for derived classes to call `IScene(manager)` in their constructor definitions.
     **/
    IScene(SceneManager& manager) : mManager(manager) {}
    virtual ~IScene() = default;

    /* Abstract interface functions */
    /**
     * @brief Event handler for the scene.
     * @param event The event to handle
     * @return void
     * 
     * Handles events for the scene.
     * ImGui events are handled in the scene manager, and are not passed to the scene.
     * @note SFML will still update state for sf::Keyboard::isKeyPressed() even if imgui has focus.
     **/
    virtual void handleEvent(const sf::Event& event) = 0;

    /**
     * @brief Update function for the scene.
     * @param dt The time since the last update
     * @return void
     * 
     * Updates the scene with a variable timestep `dt`.
     **/
    virtual void update(const sf::Time& dt) = 0;

    /**
     * @brief Draw function for the scene.
     * @param window The parent scene manager's associated window.
     * @return void
     * 
     * Draws the scene to the window.
     **/
    virtual void draw(sf::RenderWindow& window) = 0;
};

};  // namespace scenes