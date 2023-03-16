#define _USE_MATH_DEFINES
#include <components/Drawable.hpp>
#include <components/Transformable.hpp>
#include <config.hpp>
#include <imgui.h>
#include <iostream>
#include <math.h>
#include <queue>
#include <scenes.hpp>
#include <util.hpp>

namespace {

struct Item {
    std::string name;
    int value;
};

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

struct ParentedView {
    sf::RenderTarget& target;
    void set_center(sf::Vector2f center) {
        util::setViewCenter(target, center);
    }
};

struct Velocity {
    sf::Vector2f linear;
    sf::Angle angular;
};

entt::entity create_asteroid(entt::registry& registry, sf::Vector2f pos, float radius) {
    const auto asteroid_entity = registry.create();
    {
        sf::ConvexShape& asteroidShape = registry.emplace<sf::ConvexShape>(asteroid_entity, sf::ConvexShape(16));
        asteroidShape.setPosition(pos);
        asteroidShape.setTexture(&data::Textures["textures/asteroid.png"]);
        for (int i = 0; i < asteroidShape.getPointCount(); ++i) {
            float angle = static_cast<float>((i / (float)asteroidShape.getPointCount()) * 2 * M_PI);
            float r = radius * (0.8f + 0.2f * ((float)rand() / RAND_MAX));
            asteroidShape.setPoint(i, {r * cos(angle), r * sin(angle)});
        }
        // add random vel
        auto& vel = registry.get<Velocity>(asteroid_entity);
        vel.linear = {(float) (rand() % 100 - 50), (float) (rand() % 100 - 50)};
        vel.angular = sf::degrees((float) (rand() % 100 - 50));
    }
    return asteroid_entity;
}

};  // anonymous namespace

namespace scenes {

GameScene::GameScene(SceneManager& manager) : IScene(manager) {
    if (!mWorldRT.create(manager.window().getSize()))
        throw std::runtime_error("Error creating render texture");

    mFPSText.setFont(mFont);
    mFPSText.setCharacterSize(16);
    mFPSText.setFillColor(sf::Color::White);
    mFPSText.setPosition({
        config::SCREEN_WIDTH/2.f,
        0
    });

    util::add_dependent_iface<sf::Sprite, Transformable>(mRegistry);
    util::add_dependent_iface<sf::Sprite, Drawable>(mRegistry);
    util::add_dependent_iface<sf::ConvexShape, Drawable>(mRegistry);
    util::add_dependent_iface<sf::ConvexShape, Transformable>(mRegistry);
    util::add_dependent_iface<sf::Text, Drawable>(mRegistry);
    util::add_dependent<Transformable, Velocity>(mRegistry);

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

    // create some asteroids
    for (int i = 0; i < 50; ++i) {
        create_asteroid(mRegistry, {(float) (rand()%10000), (float) (rand()%10000)}, 100.f);
    }

    mManager.window().setFramerateLimit(config::MAX_FPS);
}

void GameScene::handleEvent(const sf::Event& event) {
    static struct {
        sf::Vector2i lastpos;
        bool active = false;
    } drag;
    if (mManager.window().hasFocus() == false) return;

    if (event.type == sf::Event::MouseMoved) {
        // set mCursorNearestEntity
        sf::Vector2f mouse_pos = mWorldRT.mapPixelToCoords({event.mouseMove.x, event.mouseMove.y});
        float min_dist = std::numeric_limits<float>::max();
        mRegistry.view<Transformable>().each([&](auto entity, auto& transformable) {
            float dist = (transformable.getPosition() - mouse_pos).length();
            if (dist < min_dist) {
                min_dist = dist;
                mCursorNearestEntity = entity;
            }
        });
    }

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
    mRegistry.view<Transformable, Velocity>().each(
        [dt](Transformable& transformable, Velocity& velocity) {
            transformable.move(velocity.linear * dt.asSeconds());
            transformable.rotate(velocity.angular * dt.asSeconds());
        }
    );
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
            fixed_update(std::min(config::FIXED_UPDATE_INTERVAL, elapsed));
            elapsed -= config::FIXED_UPDATE_INTERVAL;
        }
    }

    // update fps text
    mFPSText.setString(std::to_string((int)(1.f/dt.asSeconds())) + " fps");
    mFPSText.setOrigin({mFPSText.getLocalBounds().width / 2.f, 0.f});

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
        mRegistry.sort<Drawable>([](const Drawable& a, const Drawable& b) {
            return a.render_index < b.render_index;
        });
        mRegistry.view<const Drawable>().each([&](const entt::entity& ent, const Drawable& drawable) {
            if (mRegistry.all_of<Transformable>(ent)) {  // if the entity has a Transformable component, we can selectively draw it
                const Transformable& transformable = mRegistry.get<Transformable>(ent);

                auto view_rect_world = sf::FloatRect {
                    rt.getView().getCenter() - rt.getView().getSize() / 1.5f,  // top-left
                    rt.getView().getSize()*2.f  // size
                };
                if (view_rect_world.contains(transformable.getPosition()))
                    rt.draw((const sf::Drawable&)drawable);
            } else {
                rt.draw((const sf::Drawable&)drawable);
            }
        });

        {
            if (mCursorNearestEntity != entt::null) {

                sf::Vector2f transformPos = mRegistry.get<Transformable>(mCursorNearestEntity).getPosition();
                sf::Vector2f cursorWorldPos = mWorldRT.mapPixelToCoords(sf::Mouse::getPosition(window));

                sf::CircleShape circle(10.f);
                circle.setOrigin({10.f, 10.f});
                circle.setPosition(transformPos);
                circle.setFillColor(sf::Color::Transparent);
                circle.setOutlineColor(sf::Color::Red);
                circle.setOutlineThickness(2.f);
                rt.draw(circle);

                sf::Text text;
                text.setFont(mFont);
                text.setString("Entity #"+std::to_string((uint32_t)mCursorNearestEntity));
                // text.setCharacterSize(16);
                text.setFillColor(sf::Color::Green);
                text.setPosition(transformPos + sf::Vector2f{0.f, 10.f + text.getLocalBounds().height});
                rt.draw(text);

                sf::VertexArray lines(sf::PrimitiveType::LineStrip, 2);
                lines[0] = {transformPos, sf::Color::Red};
                lines[1] = {cursorWorldPos, sf::Color::Red};
                rt.draw(lines);

                if (mRegistry.all_of<Turrets>(mCursorNearestEntity)) {
                    const auto& turrets = mRegistry.get<Turrets>(mCursorNearestEntity);
                    for (const auto& turret : turrets.turrets) {
                        const auto& turretSprite = mRegistry.get<sf::Sprite>(turret.entity);
                        sf::VertexArray lines(sf::PrimitiveType::LineStrip, 2);
                        lines[0] = {transformPos, sf::Color::Green};
                        lines[1] = {turretSprite.getPosition(), sf::Color::Yellow};
                        rt.draw(lines);
                    }
                }
            }
        }

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
    window.draw(mFPSText);
}

};  // namespace scenes
