#include <scenes.hpp>
#include <config.hpp>

namespace scenes {

MenuScene::MenuScene() {
    if (!mFont.loadFromFile("assets/fonts/victor-pixel.ttf"))
        throw std::runtime_error("Error loading font: 'assets/fonts/victor-pixel.ttf'");
    
    // TODO: config::try_load(...)
    
    IScene::mWindow.create(
        sf::VideoMode({config::SCREEN_WIDTH, config::SCREEN_HEIGHT}),
        config::WINDOW_TITLE
    );
    mWindow.setFramerateLimit(60);

    mText = sf::Text("Hello World!", mFont, 30);
    mText.setPosition({100.f, 100.f});
}

void MenuScene::draw() {
    mWindow.clear();
    mWindow.draw(mText);
    mWindow.display();
}

void MenuScene::update() {
    mText.setString("Hello World! " + std::to_string(mClock.getElapsedTime().asSeconds()));
}

void MenuScene::handleEvents() {
    sf::Event event;
    while (mWindow.pollEvent(event)) {
        // ImGui::SFML::ProcessEvent(event);
        if (event.type == sf::Event::Closed) {
            mWindow.close();
        }
    }
}

};  // namespace scenes