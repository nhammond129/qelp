#include <scenes/SceneManager.hpp>
#include <config.hpp>

namespace scenes {

SceneManager::SceneManager(IScene* first_scene) {
    mScenes.push_back(std::shared_ptr<IScene>(first_scene));

    /*  TODO
        config::tryLoadConfig();    */
    mWindow.create(
        sf::VideoMode({config::SCREEN_WIDTH, config::SCREEN_HEIGHT}),
        config::WINDOW_TITLE
    );
    mWindow.setFramerateLimit(config::MAX_FPS);
}

void SceneManager::draw() {
    auto current_scene_ptr = getCurrentScene();
    mWindow.clear();
    current_scene_ptr->draw(mWindow);
    mWindow.display();
}

void SceneManager::update() {
    auto current_scene_ptr = getCurrentScene();
    current_scene_ptr->update();
}

void SceneManager::handleEvents() {
    auto current_scene_ptr = getCurrentScene();
    current_scene_ptr->handleEvents(mWindow);
}

void SceneManager::pushScene(IScene* scene) {
    mScenes.push_back(std::shared_ptr<IScene>(scene));
}

void SceneManager::popScene() {
    if (mScenes.size() > 1) {
        mScenes.pop_back();
    } else {
        throw std::runtime_error("Can't pop last scene");
    }
}

bool SceneManager::isRunning() const noexcept {
    return mRunning;
}

void SceneManager::quit() noexcept {
    mRunning = false;
}

const std::shared_ptr<IScene> SceneManager::getCurrentScene() const {
    return { mScenes.back() };
}

};  // namespace scenes