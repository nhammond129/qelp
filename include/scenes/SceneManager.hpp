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
    /**
     * @brief Constructor.
     * @param video_mode The `sf::VideoMode` to use for the window.
     * @param title The title of the window.
     * 
     * Constructs a `SceneManager` with a window of the given `video_mode` and `title`.
     * Also inits ImGui.
     **/
    SceneManager(const sf::VideoMode& video_mode, const std::string& title);
    /**
     * @brief Destructor.
     * 
     * Shuts down ImGui.
     **/
    ~SceneManager();

    /**
     * @brief Draw the current scene.
     * 
     * Calls the current scene's `draw()` method.
     **/
    void draw();
    /**
     * @brief Update the current scene.
     * 
     * Calls the current scene's `update()` method.
     **/
    void update();
    /**
     * @brief Handle events for the current scene.
     * 
     * Calls the current scene's `handleEvent()` method if the event is not consumed by ImGui.
     **/
    void handleEvents();

    /**
     * @brief Push a scene onto the stack.
     * @param scene The scene to push.
     * 
     * @note The scene is owned by the `SceneManager`
     **/
    void pushScene(IScene* scene);
    /**
     * @brief Pop the top scene off the stack.
     * 
     * @note If there is only one scene left, the `SceneManager` will quit.
     **/
    void popScene();

    sf::RenderWindow& window();

    bool isRunning() const;
    void quit();
private:
    /**
     * @brief The stack of scenes.
     */
    std::vector<std::shared_ptr<IScene>> mScenes;
    /**
     * @brief The SFML window.
     */
    sf::RenderWindow mWindow;
    /**
     * @brief The clock used for timing.
     * 
     * @note This is used for both ImGui and passed to the current
     *       scene's update method for the variable timestep.
     */
    sf::Clock mClock;

    /**
     * @brief Get the current scene.
     * @return The current scene.
     * 
     * The current scene is the top of the stack.
     * Just a shortcut for `mScenes.back()`.
     **/
    const std::shared_ptr<IScene> getCurrentScene() const;
};

};  // namespace scenes