#include <config.hpp>
#include <imgui.h>
#include <math.h>
#include <scenes.hpp>

namespace scenes {

GameScene::GameScene() {
    if (!mFont.loadFromFile("assets/fonts/victor-pixel.ttf"))
        throw std::runtime_error("Error loading font: 'assets/fonts/victor-pixel.ttf'");
}

void GameScene::handleEvent(const sf::Event& event) {}

void GameScene::update(const sf::Time& dt) {

    mTurretSprite.setPosition(mShipSprite.getPosition() + mTurretOffset.rotatedBy(mShipSprite.getRotation()));
    mTurretSprite.setRotation(mTurretSprite.getRotation() + sf::degrees(1.f));

    // if keydown 'd' move sprite to the right
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) mShipSprite.rotate(sf::degrees(static_cast<float>(90.f*dt.asSeconds())));
    // if keydown 'a' move sprite to the left
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) mShipSprite.rotate(sf::degrees(static_cast<float>(-90.f*dt.asSeconds())));
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        auto rotation = mShipSprite.getRotation();
        rotation += sf::degrees(-90.f); // rotate 'forward' vector ccw to match texture
        float x = static_cast<float>(std::cos(rotation.asRadians()) * 200.f * dt.asSeconds());
        float y = static_cast<float>(std::sin(rotation.asRadians()) * 200.f * dt.asSeconds());
        mShipSprite.move({x, y});
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        auto view = mWorldRT.getView();
        view.setCenter(mShipSprite.getPosition());
        mWorldRT.setView(view);
    }

    ImGui::ShowDemoWindow();

    ImGui::Begin("Hello, world!");
    ImGui::Text("This is some useful text.");
    ImGui::End();
}

void GameScene::draw(sf::RenderWindow& window) {
    sf::RenderTexture& rt = mWorldRT;
    rt.clear();
    {
        rt.draw(mShipSprite);
        rt.draw(mTurretSprite);
    }
    rt.display();

    window.draw(sf::Sprite(rt.getTexture()));
}

};  // namespace scenes
