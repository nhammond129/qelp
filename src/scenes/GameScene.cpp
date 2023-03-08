#include <config.hpp>
#include <imgui.h>
#include <iostream>
#include <math.h>
#include <queue>
#include <scenes.hpp>
#include <util.hpp>

namespace {

struct ComponentTest {
    int b;
};

struct ShipActionQueue {
    struct ShipAction {
        struct Move {
            sf::Vector2f pos;
        };
        enum class Type {
            Move,
        };
        Type type;
        union {
            Move move;
        };
    };
    std::queue<ShipAction> actions;
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
    mRegistry.emplace<ShipActionQueue>(entity);
}

void GameScene::handleEvent(const sf::Event& event) {
    static struct {
        sf::Vector2i lastpos;
        bool active = false;
    } drag;
    if (mManager.window().hasFocus() == false) return;

    std::optional<input::Action> maybe_action = input::getAction(event);
    if (!maybe_action) return;
    input::Action& action = *maybe_action;
    switch (action.type) {
        case input::Action::Type::ZoomIn: {
            auto view = mWorldRT.getView();
            view.zoom(1.f - action.zoom.amount * 0.1f);
            mWorldRT.setView(view);
            break;
        }
        case input::Action::Type::ZoomOut: {
            auto view = mWorldRT.getView();
            view.zoom(1.f + action.zoom.amount * 0.1f);
            mWorldRT.setView(view);
            break;
        }
        case input::Action::Type::ZoomReset: {
            auto view = mWorldRT.getView();
            view.setSize({(float)mManager.window().getSize().x, (float)mManager.window().getSize().y});
            mWorldRT.setView(view);
            break;
        }
        case input::Action::Type::MoveClick: {
            sf::Vector2f pos = mWorldRT.mapPixelToCoords(action.move.pos);
            mRegistry.view<ShipActionQueue>().each([&pos](ShipActionQueue& queue) {
                queue.actions.push( {
                    .type = ShipActionQueue::ShipAction::Type::Move,
                    .move = {pos},
                });
            });
            break;
        }
    }
    if (event.type == sf::Event::MouseMoved && drag.active) {
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

    ImGui::ShowDemoWindow();

    ImGui::Begin("Ship");
    ImGui::Text("Position: (%.2f, %.2f)", mShipSprite.getPosition().x, mShipSprite.getPosition().y);
    ImGui::Text("Rotation (deg): %.2f", mShipSprite.getRotation().asDegrees());
    ImGui::End();

    ImGui::Begin("ComponentTest instances");
    mRegistry.view<const ComponentTest>().each([](const auto& entity, const auto& component) {
        ImGui::Text("Entity %d: %d", entity, component.b);
    });
    ImGui::End();

    ImGui::Begin("ShipActionQueue instances");
    mRegistry.view<const ShipActionQueue>().each([](const auto& entity, const auto& queue) {
        ImGui::Text("Entity %d: %d actions deep", entity, queue.actions.size());
    });
    ImGui::End();
}

void GameScene::draw(sf::RenderWindow& window) {
    sf::RenderTexture& rt = mWorldRT;
    rt.clear();
    {
        mRegistry.view<const ShipActionQueue>().each([this](const auto& queue) {
            sf::RenderTexture& rt = this->mWorldRT;
            if (queue.actions.empty()) return;
            std::queue<ShipActionQueue::ShipAction> copyq = queue.actions;
            sf::Vector2f lastpos = mShipSprite.getPosition();
            while (!copyq.empty()) {
                auto action = copyq.front();
                switch (action.type) {
                    case ShipActionQueue::ShipAction::Type::Move: {
                        // draw line from 'ship' to position
                        sf::VertexArray line(sf::PrimitiveType::Lines, 2);
                        line[0].position = lastpos;
                        line[1].position = action.move.pos;
                        line[0].color = sf::Color::Green;
                        line[1].color = sf::Color::Green;
                        rt.draw(line);
                        // draw circle at position
                        sf::CircleShape circle(5.f);
                        circle.setOrigin({5.f, 5.f});
                        circle.setPosition(action.move.pos);
                        circle.setFillColor(sf::Color::Transparent);
                        circle.setOutlineColor(sf::Color::Green);
                        circle.setOutlineThickness(1.0f);
                        rt.draw(circle);
                        lastpos = action.move.pos;
                        break;
                    }
                }
                copyq.pop();
            }
        });
        rt.draw(mShipSprite);
        rt.draw(mTurretSprite);
    }
    rt.display();

    window.draw(sf::Sprite(rt.getTexture()));
}

};  // namespace scenes
