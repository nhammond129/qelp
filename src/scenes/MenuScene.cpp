#include <scenes.hpp>
#include <config.hpp>
#include <imgui.h>

namespace scenes {

MenuScene::MenuScene() {
    if (!mFont.loadFromFile("assets/fonts/victor-pixel.ttf"))
        throw std::runtime_error("Error loading font: 'assets/fonts/victor-pixel.ttf'");

    mText = sf::Text("Hello World!", mFont, 30);
    mText.setPosition({100.f, 100.f});
}

void MenuScene::handleEvent(const sf::Event& event) {}

void MenuScene::update(const sf::Time& dt) {
    mText.setString("Hello World! " + std::to_string(1/(dt.asSeconds())) + " FPS");

	ImGui::Begin("Hello, world!");
	ImGui::Text("This is some useful text.");
	ImGui::End();
}

void MenuScene::draw(sf::RenderWindow& window) {
    window.draw(mText);
}

};  // namespace scenes
