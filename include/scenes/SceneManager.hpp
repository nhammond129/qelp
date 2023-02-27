#pragma once

#include <SFML/Graphics.hpp>
#include <IScene.hpp>

namespace scenes {

/* Manages a stack of scenes and handles switching between them
 * 
 * Index `0` of `mScenes` is the bottom of the stack
 * Index `mScenes.size() - 1` is the top of the stack
 * Also handles the game loop components via `draw()`, `update()`, and `handleEvents()`
 * Singleton class
 * 
 * call ordering:
 *   Input -> Update -> Draw
 */
class SceneManager {
public:
    SceneManager(IScene* first_scene);
    ~SceneManager();

    // no copy, no assign -- SINGLETON
    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;

    void draw();
    void update();
    void handleEvents();

    void pushScene(IScene* scene);
    void popScene();

    bool isRunning() const;
    static void quit();

private:
    inline static bool mRunning = true;
    static std::vector<std::shared_ptr<IScene>> mScenes;

    const std::shared_ptr<IScene> getCurrentScene() const;
};

};  // namespace scenes