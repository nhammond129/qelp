#include <config.hpp>
#include <imgui.h>
#include <iostream>
#include <math.h>
#include <queue>
#include <scenes.hpp>
#include <util.hpp>

namespace {

struct PlayerActionQueue {
    struct Action {
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
    std::list<Action> actions;
};

struct Turrets {
    struct Turret {
        entt::entity entity;
        sf::Vector2f offset;
    };
    std::vector<Turret> turrets;
};

struct ParentedView {
    sf::RenderTarget& target;
    void set_center(sf::Vector2f center) {
        util::setViewCenter(target, center);
    }
};

};  // anonymous namespace

namespace scenes {

GameScene::GameScene(SceneManager& manager) : IScene(manager) {
    if (!mWorldRT.create(manager.window().getSize()))
        throw std::runtime_error("Error creating render texture");
    if (!mFont.loadFromFile("assets/fonts/victor-pixel.ttf"))
        throw std::runtime_error("Error loading font: 'assets/fonts/victor-pixel.ttf'");

    const auto turret_entity = mRegistry.create();
    {
        sf::Sprite& turretSprite = mRegistry.emplace<sf::Sprite>(turret_entity, util::programmerArtTexture(32, 32, sf::Color::Yellow));
        auto tx_size = turretSprite.getTexture()->getSize();
        turretSprite.setOrigin({tx_size.x / 2.f, tx_size.y / 2.f});
    }

    const auto player_ship = mRegistry.create();
    {
        mRegistry.emplace<PlayerActionQueue>(player_ship);
        mRegistry.emplace<ParentedView>(player_ship, mWorldRT);
        sf::Sprite& mShipSprite = mRegistry.emplace<sf::Sprite>(player_ship, util::programmerArtTexture(128, 96, sf::Color::Magenta));
        auto tx_size = mShipSprite.getTexture()->getSize();
        mShipSprite.setOrigin({tx_size.x / 2.f, tx_size.y / 2.f});

        Turrets& turrets = mRegistry.emplace<Turrets>(player_ship);
        turrets.turrets.push_back({turret_entity, {mShipSprite.getLocalBounds().width * -0.3f, 0.f}});
    }


}

void GameScene::handleEvent(const sf::Event& event) {
    static struct {
        sf::Vector2i lastpos;
        bool active = false;
    } drag;
    if (mManager.window().hasFocus() == false) return;

    std::optional<input::Action> maybe_action = input::getAction(event);
    if (maybe_action) {
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
                mRegistry.view<PlayerActionQueue>().each([&pos](PlayerActionQueue& queue) {
                    queue.actions.push_back( {
                        .type = PlayerActionQueue::Action::Type::Move,
                        .move = {pos},
                    });
                });
                break;
            }
        }
    }

    // drag-click logic
    // TODO: move this to input::getAction ?
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Button::Middle) {
        drag.active = true;
        drag.lastpos = {event.mouseButton.x, event.mouseButton.y};
    } else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Button::Middle) {
        drag.active = false;
    } else if (event.type == sf::Event::MouseMoved && drag.active) {
        sf::Vector2f motionDelta = mWorldRT.mapPixelToCoords(drag.lastpos) - mWorldRT.mapPixelToCoords({event.mouseMove.x, event.mouseMove.y});
        util::moveViewCenter(mWorldRT, motionDelta);
        drag.lastpos = {event.mouseMove.x, event.mouseMove.y};
    }
}

void GameScene::update(const sf::Time& dt) {
    static bool doTrack = true;
    util::RAIITimed raii("update");
    { util::RAIITimed raii("update::movement");
    mRegistry.view<PlayerActionQueue, sf::Sprite>().each([this, dt](PlayerActionQueue& queue, sf::Sprite& sprite) {
        // Iterating components that both have a PlayerActionQueue and a sf::Sprite.
        if (queue.actions.empty()) return;
        auto& action = queue.actions.front();
        switch (action.type) {
            case PlayerActionQueue::Action::Type::Move: {
                auto& move = action.move;
                auto to = move.pos;
                auto from = sprite.getPosition();
                auto delta = to - from;
                auto dist = std::sqrt(delta.x * delta.x + delta.y * delta.y);
                if (dist < 1.f) {
                    queue.actions.pop_front();
                    break;
                }
                auto dir = delta / dist;
                auto speed = 100.f;
                auto moveDelta = dir * speed * dt.asSeconds();
                sprite.move(moveDelta);
                sprite.setRotation(sf::radians(std::atan2(dir.y, dir.x)));
                break;
            }
        }
    });
    }

    { util::RAIITimed raii("update::turrets");
    mRegistry.view<Turrets, sf::Sprite>().each([this, dt](Turrets& turrets, sf::Sprite& sprite) {
        for (auto& turret : turrets.turrets) {
            auto& turretSprite = mRegistry.get<sf::Sprite>(turret.entity);
            turretSprite.setPosition(sprite.getPosition() + turret.offset.rotatedBy(sprite.getRotation()));
            float turretAngle;
            {
                sf::Vector2i mousePos = sf::Mouse::getPosition(mManager.window());
                sf::Vector2f turretPos = turretSprite.getPosition();
                auto turretToMouse = mWorldRT.mapPixelToCoords(mousePos) - turretPos;
                turretAngle = std::atan2(turretToMouse.y, turretToMouse.x);
            }
            turretSprite.setRotation(sf::radians(turretAngle));
        }
    });
    }

    { util::RAIITimed raii("update::view_tracking");
    if (doTrack) {
        mRegistry.view<ParentedView, sf::Sprite>().each([this](ParentedView& view, sf::Sprite& sprite) {
            view.set_center(sprite.getPosition());
        });
    }
    }

    { util::RAIITimed raii("update::debug");

    // just quickly set position for the debug window
    ImGui::SetNextWindowPos({(float)config::SCREEN_WIDTH, (float)config::SCREEN_HEIGHT/2.f}, ImGuiCond_Once, {1.f, 0.5f});
    ImGui::Begin("debug"); ImGui::End();


    ImGui::SetNextWindowPos({(float)config::SCREEN_WIDTH, 0.f}, ImGuiCond_Once, {1.f, 0.f});
    ImGui::Begin("view tracking");
    ImGui::Checkbox("Track player ship", &doTrack);
    ImGui::End();

    ImGui::Begin("PlayerActionQueue instances");
    mRegistry.view<const PlayerActionQueue>().each([](const auto entity, const auto& queue) {
        bool open = ImGui::TreeNode(std::format("Entity {}", (uint32_t)entity).c_str());
        ImGui::Text("Queue depth: %d", queue.actions.size());
        if (open) {
            for (const auto& action : queue.actions) {
                switch (action.type) {
                    case PlayerActionQueue::Action::Type::Move: {
                        ImGui::Text("Move to (%f, %f)", action.move.pos.x, action.move.pos.y);
                        break;
                    }
                }
            }
            ImGui::TreePop();
        }
    });
    ImGui::End();
    }
}

void GameScene::draw(sf::RenderWindow& window) {
    util::RAIITimed raii("draw");
    sf::RenderTexture& rt = mWorldRT;
    rt.clear();
    {
        util::RAIITimed raii("draw::world");
        { util::RAIITimed raii("draw::world::sprites");
        mRegistry.view<const sf::Sprite>().each([&rt](const auto& sprite) {
            rt.draw(sprite);
        });
        }
        { util::RAIITimed raii("draw::world::debug");
        mRegistry.view<const PlayerActionQueue, const sf::Sprite>().each([this](const auto& queue, const auto& sprite) {
            sf::RenderTexture& rt = this->mWorldRT;
            if (queue.actions.empty()) return;
            sf::Vector2f lastpos = sprite.getPosition();
            for (const auto& action : queue.actions) {
                switch (action.type) {
                    case PlayerActionQueue::Action::Type::Move: {
                        // draw line from 'ship' to position
                        util::RAIITimed* tmp = new util::RAIITimed("draw::world::debug (sf::VertexArray c'tor)");
                        sf::VertexArray line(sf::PrimitiveType::Lines, 2);
                        delete tmp;
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
            }
        });
        }
    }
    rt.display();

    window.draw(sf::Sprite(rt.getTexture()));
}

};  // namespace scenes
