#include <scenes.hpp>
#include <config.hpp>
#include <imgui.h>

namespace scenes {

MenuScene::MenuScene(SceneManager& manager) : IScene(manager) {
    if (!mFont.loadFromFile("assets/fonts/victor-pixel.ttf"))
        throw std::runtime_error("Error loading font: 'assets/fonts/victor-pixel.ttf'");

    mText = sf::Text("Hello World!", mFont, 30);
    mText.setPosition({100.f, 100.f});
}

void MenuScene::handleEvent(const sf::Event& event) {
    if (mManager.window().hasFocus() == false) return;
    
    if (event.type == sf::Event::KeyReleased) {
        if (event.key.code == sf::Keyboard::G) {
            mManager.pushScene(new GameScene(mManager));
        }
    }
}

void MenuScene::update(const sf::Time& dt) {
    mText.setString("Hello World! " + std::to_string(1/(dt.asSeconds())) + " FPS");

    ImGui::ShowDemoWindow();
}

void MenuScene::draw(sf::RenderWindow& window) {
    window.draw(mText);
}

};  // namespace scenes
