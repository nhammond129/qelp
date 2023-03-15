#include <config.hpp>
#include <imgui.h>
#include <iostream>
#include <math.h>
#include <queue>
#include <scenes.hpp>
#include <util.hpp>

namespace {

struct PlayerActionQueue {
    static const uint32_t MAX_DEPTH = 64;
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

struct Drawable {
    sf::Drawable& drawable;
};

struct Transformable {
    sf::Transformable& transformable;
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

    // construct Transformable components from an existing sf::Sprite component
    // in hindsight, this is a little cursed. Cute, but cursed.
    // I should just stick to more conventional components.
    // Also, Transformable and Drawable could 
    util::add_dependent_iface<sf::Sprite, Transformable>(mRegistry);
    util::add_dependent_iface<sf::Sprite, Drawable>(mRegistry);       // and similarly for Drawable

    auto turretBuilder = [this]() {
        const auto turret_entity = mRegistry.create();
        {
            sf::Sprite& turretSprite = mRegistry.emplace<sf::Sprite>(turret_entity, data::Textures["sprites/turret1.png"]);
            auto tx_size = turretSprite.getTexture()->getSize();
            turretSprite.setOrigin({18, 20});
        }
        return turret_entity;
    };

    sf::Sprite shipSprite(data::Textures["sprites/ship1.png"]);
    auto tx_size = shipSprite.getTexture()->getSize();
    shipSprite.setOrigin({tx_size.x / 2.f, tx_size.y / 2.f});

    std::vector<Turrets::Turret> turrets = {
        {turretBuilder(), (sf::Vector2f {168,  71} - shipSprite.getOrigin())},
        {turretBuilder(), (sf::Vector2f {263,  27} - shipSprite.getOrigin())},
        {turretBuilder(), (sf::Vector2f {168, 150} - shipSprite.getOrigin())},
        {turretBuilder(), (sf::Vector2f {263, 195} - shipSprite.getOrigin())},
        {turretBuilder(), (sf::Vector2f {450,  23} - shipSprite.getOrigin())}
    };

    const auto player_ship = mRegistry.create();
    {
        mRegistry.emplace<PlayerActionQueue>(player_ship);
        mRegistry.emplace<ParentedView>(player_ship, mWorldRT);
        mRegistry.emplace<sf::Sprite>(player_ship, shipSprite);
        mRegistry.emplace<Turrets>(player_ship, turrets);
    }

    mManager.window().setFramerateLimit(config::MAX_FPS);
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
                    if (queue.actions.size() > PlayerActionQueue::MAX_DEPTH)
                        queue.actions.pop_front();
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

void GameScene::fixed_update(const sf::Time& dt) {
    mRegistry.view<PlayerActionQueue, sf::Sprite>().each([this, dt](PlayerActionQueue& queue, sf::Sprite& sprite) {
        // Iterating components that both have a PlayerActionQueue and an sf::Sprite.
        if (queue.actions.empty()) return;
        auto& action = queue.actions.front();
        switch (action.type) {
            case PlayerActionQueue::Action::Type::Move: {
                const auto to = action.move.pos;
                const auto from = sprite.getPosition();
                const auto delta = to - from;
                const auto dist = std::sqrt(delta.x * delta.x + delta.y * delta.y);
                if (dist < 5.f) {
                    queue.actions.pop_front();
                    break;
                }
                const auto dir = delta / dist;
                const auto speed = 100.f;
                const auto moveDelta = dir * speed * dt.asSeconds();
                sprite.move(moveDelta);
                sprite.setRotation(sf::radians(std::atan2(dir.y, dir.x)));
                break;
            }
        }
    });
}

void GameScene::update(const sf::Time& dt) {
    {   // fixed update (physics)
        sf::Time elapsed = dt;
        while (elapsed > sf::Time::Zero) {
            fixed_update(std::min(mFixedUpdateInterval, elapsed));
            elapsed -= mFixedUpdateInterval;
        }
    }
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

    if (mViewTracking) {
        mRegistry.view<ParentedView, sf::Sprite>().each([this](ParentedView& view, sf::Sprite& sprite) {
            view.set_center(sprite.getPosition());
        });
    }
    // just quickly set position for the debug window
    ImGui::SetNextWindowPos({(float)config::SCREEN_WIDTH, (float)config::SCREEN_HEIGHT/2.f}, ImGuiCond_Once, {1.f, 0.5f});
    ImGui::Begin("debug"); ImGui::End();


    ImGui::SetNextWindowPos({(float)config::SCREEN_WIDTH, 0.f}, ImGuiCond_Once, {1.f, 0.f});
    ImGui::Begin("view tracking");
    ImGui::Checkbox("Track player ship", &mViewTracking);
    ImGui::End();

    ImGui::Begin("PlayerActionQueue instances");
    mRegistry.view<const PlayerActionQueue>().each([](const auto entity, const auto& queue) {
        bool open = ImGui::TreeNode(("Entity #" + std::to_string((uint32_t)entity)).c_str());
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

void GameScene::draw(sf::RenderWindow& window) {
    sf::RenderTexture& rt = mWorldRT;
    rt.clear();
    {
        // TODO: z-indexing for sprites
        mRegistry.view<const Drawable>().each([&](const auto& drawable) {
            rt.draw(drawable.drawable);
        });

        mRegistry.view<const PlayerActionQueue, const sf::Sprite>().each([this](const auto& queue, const auto& sprite) {
            sf::RenderTexture& rt = this->mWorldRT;
            if (queue.actions.empty()) return;
            sf::VertexArray lines(sf::PrimitiveType::LineStrip, queue.actions.size() + 1);
            lines[0] = {sprite.getPosition(), sf::Color::Green};
            int idx = 1;
            for (const auto& action : queue.actions) {
                switch (action.type) {
                    case PlayerActionQueue::Action::Type::Move: {
                        // draw line from 'ship' to position
                        lines[idx++] = {action.move.pos, sf::Color::Green};
                        break;
                    }
                }
            }
            rt.draw(lines);
        });
    }
    rt.display();
    window.draw(sf::Sprite(rt.getTexture()));
}

};  // namespace scenes
