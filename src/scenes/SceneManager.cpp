#include <scenes.hpp>
#include <config.hpp>

namespace scenes {

std::vector<std::shared_ptr<IScene>> SceneManager::mScenes;

SceneManager::SceneManager(IScene* first_scene) {
    mScenes.push_back(std::shared_ptr<IScene>(first_scene));
    IScene::init_imgui();
}

SceneManager::~SceneManager() {
    IScene::shutdown_imgui();
}

void SceneManager::handleEvents() {
    auto current_scene_ptr = getCurrentScene();
    current_scene_ptr->handleEvents();
}

void SceneManager::update() {
    IScene::update_imgui();  // this must happen first if we want imgui calls in scene's update()
    auto current_scene_ptr = getCurrentScene();
    current_scene_ptr->update();
}

void SceneManager::draw() {
    auto current_scene_ptr = getCurrentScene();
    current_scene_ptr->draw();
}

void SceneManager::pushScene(IScene* scene) {
    mScenes.push_back(std::shared_ptr<IScene>(scene));
}

void SceneManager::popScene() {
    if (mScenes.size() > 1) {
        mScenes.pop_back();
    } else { // if there is only one scene left, just quit
        quit();
    }
}

bool SceneManager::isRunning() const {
    return mRunning;
}

void SceneManager::quit() {
    mRunning = false;
}

const std::shared_ptr<IScene> SceneManager::getCurrentScene() const {
    return { mScenes.back() };
}

};  // namespace scenes
