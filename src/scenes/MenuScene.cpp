#include <scenes/MenuScene.hpp>

namespace scenes {

MenuScene::MenuScene() {
    if (!mFont.loadFromFile("assets/fonts/victor-pixel.ttf"))
        throw std::runtime_error("Error loading font: 'assets/fonts/victor-pixel.ttf'");
    

    mText = sf::Text("Hello World!", mFont, 30);
    mText.setPosition({100.f, 100.f});
}

void MenuScene::draw(sf::RenderWindow& window) {
    window.draw(mText);
}

void MenuScene::update() {
    mText.setString("Hello World! " + std::to_string(mClock.getElapsedTime().asSeconds()));
}

void MenuScene::handleEvents(sf::RenderWindow& window) {
    sf::Event event;
    while (window.pollEvent(event)) {
        // ImGui::SFML::ProcessEvent(event);
        if (event.type == sf::Event::Closed) {
            window.close();
        }
    }
}

};  // namespace scenes