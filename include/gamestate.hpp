#pragma once
#include <box2d/box2d.h>
#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

namespace net {
    struct StateUpdate;
};  // namespace net

namespace game {

struct PlayerDataComponent {
    uint32_t shipdata;
};

struct b2BodyComponent {
    struct RenderingInfo {
        sf::ConvexShape shape;
    };
    b2Body* body = nullptr;
    std::optional<RenderingInfo> renderingInfo = std::nullopt;
    void draw(sf::RenderTarget& target, sf::RenderStates states);
};

struct shipState {
    struct thruster {
        bool active = false;
        float force = 0.f; 
        b2Vec2 pos; // relative to ship
        b2Vec2 dir; // should always be normalized
    };
    std::vector<thruster> mThrusters;
};

class State {
public:
    State();
    ~State() = default;

    void update(const sf::Time& dt);

    entt::entity makeAsteroid(const sf::Vector2f& pos, float radius);  // UNTESTED
    entt::entity createPlayer(const sf::Vector2f& position);
    entt::registry& getRegistry() { return mRegistry; }

    net::StateUpdate getFullStateUpdate();
private:
    b2World mWorld;
    entt::registry mRegistry;
};

class LocalState: public State {
public:
    LocalState(): State() {};
    ~LocalState() = default;

    entt::entity getPlayer() { return mPlayer; }

private:
    entt::entity mPlayer = entt::null;
};

};  // namespace game