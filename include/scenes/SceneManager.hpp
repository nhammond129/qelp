#pragma once

#include <SFML/Graphics.hpp>
#include <IScene.hpp>

namespace scenes {

/* Manages a stack of scenes and handles switching between them
 * 
 * Tied to the lifetime of an sf::RenderWindow
 * Index `0` of `mScenes` is the bottom of the stack
 * Index `mScenes.size() - 1` is the top of the stack
 * Also handles the game loop components via `draw()`, `update()`, and `handleEvents()`
 * 
 * call ordering:
 *   Input -> Update -> Draw
 */
class SceneManager {
public:
    SceneManager(const sf::VideoMode& video_mode, const std::string& title);
    ~SceneManager();

    void draw();
    void update();
    void handleEvents();

    void pushScene(IScene* scene);
    void popScene();

    sf::RenderWindow& window();

    bool isRunning() const;
    void quit();
private:
    std::vector<std::shared_ptr<IScene>> mScenes;
    sf::RenderWindow mWindow;
    sf::Clock mClock;

    const std::shared_ptr<IScene> getCurrentScene() const;
};

};  // namespace scenes