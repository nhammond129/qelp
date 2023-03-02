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
    sf::Clock mClock;
    SceneManager& mManager;
public:
    IScene() = delete;
    IScene(SceneManager& manager) : mManager(manager) {}
    virtual ~IScene() = default;

    /* Abstract interface functions */
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual void update(const sf::Time& dt) = 0;
    virtual void handleEvent(const sf::Event& event) = 0;
};

};  // namespace scenes