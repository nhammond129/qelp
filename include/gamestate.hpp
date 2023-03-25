#pragma once
#include <box2d/box2d.h>
#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

namespace game {

struct PlayerData {
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
    explicit State();
    ~State() = default;

    void update(const sf::Time& dt);

    entt::entity createPlayer(const sf::Vector2f& position);
    entt::registry& getRegistry() { return mRegistry; }
private:
    b2World mWorld;
    entt::registry mRegistry;
};

};  // namespace game