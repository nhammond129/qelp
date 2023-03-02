#include <config.hpp>
#include <imgui.h>
#include <math.h>
#include <scenes.hpp>
#include <util.hpp>

namespace {

struct ComponentTest {
    int b;
};

};  // anonymous namespace

namespace scenes {

GameScene::GameScene(SceneManager& manager) : IScene(manager) {
    if (!mWorldRT.create(manager.window().getSize()))
        throw std::runtime_error("Error creating render texture");
    if (!mFont.loadFromFile("assets/fonts/victor-pixel.ttf"))
        throw std::runtime_error("Error loading font: 'assets/fonts/victor-pixel.ttf'");

    mShipTexture = util::programmerArtTexture(128, 96, sf::Color::Magenta);
    mShipSprite.setTexture(mShipTexture);
    mShipSprite.setOrigin({mShipTexture.getSize().x / 2.f, mShipTexture.getSize().y / 2.f});

    mShipTurretTexture = util::programmerArtTexture(32, 32, sf::Color::Yellow);
    mTurretSprite.setTexture(mShipTurretTexture);
    mTurretSprite.setOrigin({mShipTurretTexture.getSize().x / 2.f, mShipTurretTexture.getSize().y / 2.f});

    mTurretOffset = {-mShipSprite.getLocalBounds().width * 0.3f, 0.f};

    auto view = mWorldRT.getView();
    view.setCenter(mShipSprite.getPosition());
    mWorldRT.setView(view);

    const auto entity = mRegistry.create();
    mRegistry.emplace<ComponentTest>(entity, 69420);
}

void GameScene::handleEvent(const sf::Event& event) {
    static struct {
        sf::Vector2i lastpos;
        bool active = false;
    } drag;
    if (mManager.window().hasFocus() == false) return;

    if (event.type == sf::Event::KeyReleased) {
        if (event.key.code == sf::Keyboard::Escape) {
            mManager.popScene();
        }
    } else if (event.type == sf::Event::MouseWheelScrolled) {
        auto view = mWorldRT.getView();
        view.zoom(1.f - event.mouseWheelScroll.delta * 0.1f);
        mWorldRT.setView(view);
    } else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Middle) {
        drag.lastpos = {event.mouseButton.x, event.mouseButton.y};
        drag.active = true;
    } else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Middle) {
        drag.active = false;
    } else if (event.type == sf::Event::MouseMoved && drag.active) {
        auto view = mWorldRT.getView();
        sf::Vector2f motionDelta = mWorldRT.mapPixelToCoords(drag.lastpos) - mWorldRT.mapPixelToCoords({event.mouseMove.x, event.mouseMove.y});
        view.move(motionDelta);
        mWorldRT.setView(view);
        drag.lastpos = {event.mouseMove.x, event.mouseMove.y};
    }
}

void GameScene::update(const sf::Time& dt) {

    // angle between turret and cursor
    float turretAngle;
    {
        sf::Vector2i mousePos = sf::Mouse::getPosition(mManager.window());
        sf::Vector2f turretPos = mTurretSprite.getPosition();
        auto turretToMouse = mWorldRT.mapPixelToCoords(mousePos) - turretPos;
        turretAngle = std::atan2(turretToMouse.y, turretToMouse.x);
    }
    mTurretSprite.setPosition(mShipSprite.getPosition() + mTurretOffset.rotatedBy(mShipSprite.getRotation()));
    mTurretSprite.setRotation(sf::radians(turretAngle));

    /**
     * only move if window has focus
     * but using sf::Keyboard::isKeyPressed() ignores ImGuiIO::WantCaptureKeyboard flag
     *
     * In the future, should probably keep our own input table in-scene
     * based on what gets passed through IScene::handleEvent
     **/
    if (mManager.window().hasFocus()) {
        // if keydown 'd' move sprite to the right
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) mShipSprite.rotate(sf::degrees(static_cast<float>(90.f*dt.asSeconds())));
        // if keydown 'a' move sprite to the left
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) mShipSprite.rotate(sf::degrees(static_cast<float>(-90.f*dt.asSeconds())));
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            auto rotation = mShipSprite.getRotation();
            float x = static_cast<float>(std::cos(rotation.asRadians()) * 200.f * dt.asSeconds());
            float y = static_cast<float>(std::sin(rotation.asRadians()) * 200.f * dt.asSeconds());
            mShipSprite.move({x, y});
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            auto view = mWorldRT.getView();
            view.setCenter(mShipSprite.getPosition());
            mWorldRT.setView(view);
        }
    }

    ImGui::ShowDemoWindow();

    ImGui::Begin("Ship");
    ImGui::Text("Position: (%.2f, %.2f)", mShipSprite.getPosition().x, mShipSprite.getPosition().y);
    ImGui::Text("Rotation (deg): %.2f", mShipSprite.getRotation().asDegrees());
    ImGui::End();

    ImGui::Begin("ComponentTest instances");
    mRegistry.view<ComponentTest>().each([](const auto& entity, const auto& component) {
        ImGui::Text("Entity %d: %d", entity, component.b);
    });
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
