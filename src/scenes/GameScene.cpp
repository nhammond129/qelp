#include <config.hpp>
#include <imgui.h>
#include <math.h>
#include <scenes.hpp>

namespace scenes {

GameScene::GameScene() {
    if (!mFont.loadFromFile("assets/fonts/victor-pixel.ttf"))
        throw std::runtime_error("Error loading font: 'assets/fonts/victor-pixel.ttf'");

    IScene::mWindow.create(
        sf::VideoMode({config::SCREEN_WIDTH, config::SCREEN_HEIGHT}),
        config::WINDOW_TITLE
    );
    mWindow.setFramerateLimit(config::MAX_FPS);
}

void GameScene::draw() {
    sf::RenderTexture& rt = mWorldRT;
    rt.clear();
    {
        rt.draw(mShipSprite);
        rt.draw(mTurretSprite);
    }
    rt.display();

    mWindow.clear();
    {
        mWindow.draw(sf::Sprite(rt.getTexture()));
        draw_imgui();
    }
    mWindow.display();
}

void GameScene::update() {
    float dt = mClock.restart().asSeconds();

    mTurretSprite.setPosition(mShipSprite.getPosition() + mTurretOffset.rotatedBy(mShipSprite.getRotation()));
    mTurretSprite.setRotation(mTurretSprite.getRotation() + sf::degrees(1.f));

    // if keydown 'd' move sprite to the right
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) mShipSprite.rotate(sf::degrees(static_cast<float>(90.f*dt)));
    // if keydown 'a' move sprite to the left
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) mShipSprite.rotate(sf::degrees(static_cast<float>(-90.f*dt)));
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        auto rotation = mShipSprite.getRotation();
        rotation += sf::degrees(-90.f); // rotate 'forward' vector ccw to match texture
        float x = static_cast<float>(std::cos(rotation.asRadians()) * 200.f * dt);
        float y = static_cast<float>(std::sin(rotation.asRadians()) * 200.f * dt);
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

void GameScene::handleEvents() {
    sf::Event event;
    while (mWindow.pollEvent(event)) {
        IScene::handleEvent_imgui(event);
        if (event.type == sf::Event::Closed) {
            mWindow.close();
            SceneManager::quit();
        }
    }
}

};  // namespace scenes
