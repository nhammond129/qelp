#include <scenes.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <config.hpp>

namespace scenes {

SceneManager::SceneManager(IScene* first_scene, const sf::VideoMode& video_mode, const std::string& title) {
    mScenes.push_back(std::shared_ptr<IScene>(first_scene));

    mWindow.create(video_mode, title);

    if (!ImGui::SFML::Init(mWindow)) {
        throw std::runtime_error("Failed to initialize ImGui");
    }
}

SceneManager::~SceneManager() {
    ImGui::SFML::Shutdown(mWindow);
}

void SceneManager::handleEvents() {
    sf::Event event;
    while (mWindow.pollEvent(event)) {
        ImGui::SFML::ProcessEvent(mWindow, event);
        if (event.type == sf::Event::Closed) {
            mWindow.close();
        } else {
            auto current_scene_ptr = getCurrentScene();
            current_scene_ptr->handleEvent(event);
        }
    }
}

void SceneManager::update() {
    sf::Time elapsed = mClock.restart();
    ImGui::SFML::Update(mWindow, elapsed);
    auto current_scene_ptr = getCurrentScene();
    current_scene_ptr->update(elapsed);
}

void SceneManager::draw() {
    mWindow.clear();
    auto current_scene_ptr = getCurrentScene();
    current_scene_ptr->draw(mWindow);
    ImGui::SFML::Render(mWindow);
    mWindow.display();
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
    return mWindow.isOpen();
}

void SceneManager::quit() {
    mWindow.close();
    ImGui::SFML::Shutdown(mWindow);
}

const std::shared_ptr<IScene> SceneManager::getCurrentScene() const {
    return { mScenes.back() };
}

};  // namespace scenes
