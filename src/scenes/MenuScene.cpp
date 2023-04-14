#include <config.hpp>
#include <imgui.h>
#include <iostream>
#include <scenes.hpp>

namespace scenes {

MenuScene::MenuScene(SceneManager& manager) : IScene(manager) {
    mOptions = {
        Option(sf::Text("Play (local server)", mFont, 30), [this](){
            mManager.pushScene(new GameClient(mManager));
        }),
        Option(sf::Text("Settings", mFont, 30), [this](){
            // TODO: Settings menu
            std::cout << "<Settings>" << std::endl;
        }),
        Option(sf::Text("Exit", mFont, 30), [this](){
            mManager.quit();
        }),
    };

    mManager.window().setFramerateLimit(60);
}

void MenuScene::handleEvent(const sf::Event& event) {
    if (mManager.window().hasFocus() == false) return;

    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f pos = {(float)event.mouseMove.x, (float)event.mouseMove.y};
        for (auto& opt : mOptions) {
            if (opt.text.getGlobalBounds().contains(pos))
                opt.text.setString("> " + opt.name + " <");
            else 
                opt.text.setString(opt.name);
        }
    } else if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2f pos = {(float)event.mouseButton.x, (float)event.mouseButton.y};
        for (auto& opt : mOptions) {
            if (opt.text.getGlobalBounds().contains(pos)) {
                opt.hot = true;
                opt.text.setFillColor(sf::Color::Red);
            }
        }
    } else if (event.type == sf::Event::MouseButtonReleased) {
        sf::Vector2f pos = {(float)event.mouseButton.x, (float)event.mouseButton.y};
        for (auto& opt : mOptions) {
            if (opt.text.getGlobalBounds().contains(pos) && opt.hot) {
                opt.callback();
            }
            opt.hot = false;
            opt.text.setFillColor(sf::Color::White);
        }
    }
}

void MenuScene::update(const sf::Time& dt) {
    sf::Vector2f anchor = mAnchor;
    for (auto& opt : mOptions) {
        opt.text.setPosition(anchor);
        anchor.y += opt.text.getGlobalBounds().height + mSpacing;
    }
    ImGui::ShowDemoWindow();
}

void MenuScene::draw(sf::RenderWindow& window) {
    for (auto& opt : mOptions) {
        window.draw(opt.text);
    }
}

};  // namespace scenes
