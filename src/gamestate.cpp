#include <SFML/System/Vector2.hpp>
#include <box2d/box2d.h>
#include <gamestate.hpp>
#include <net/packets.hpp>
#include <data/cached.hpp>

namespace game {

void b2BodyComponent::draw(sf::RenderTarget& target, sf::RenderStates states) {
    if (!renderingInfo.has_value()) return;
    RenderingInfo& info = renderingInfo.value();

    // update shape data
    info.shape.setPosition(sf::Vector2f {body->GetPosition().x, body->GetPosition().y});
    info.shape.setRotation(sf::radians(body->GetAngle()));

    target.draw(info.shape, states);
}

State::State(): mWorld(b2Vec2(0.f, 0.f)) {}

void State::update(const sf::Time& dt) {
    const int32_t CVelocityIterations = 4;
    const int32_t CPositionIterations = 4;
    mWorld.Step(dt.asSeconds(), CVelocityIterations, CPositionIterations);
}

entt::entity State::makeAsteroid(const sf::Vector2f& pos, float radius) {
    entt::entity entity = mRegistry.create();

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = b2Vec2(pos.x, pos.y);

    b2Body* body = mWorld.CreateBody(&bodyDef);

    // add N=16 vertices, with a random radius between `radius` and `radius + 5`
    b2Vec2 vertices[16];
    for (int i = 0; i < 16; ++i) {
        float angle = (float)i / 16.f * 2.f * b2_pi;
        float r = radius + 5 * (float)rand() / RAND_MAX;
        vertices[i] = b2Vec2 {r * cos(angle), r * sin(angle)};
    }
    b2PolygonShape b2shape;
    body->CreateFixture(&b2shape, 2.0f/* density */);

    b2BodyComponent bodyComponent {
        .body = body,
        .renderingInfo = b2BodyComponent::RenderingInfo {
            .shape = sf::ConvexShape(16)
        }
    };
    bodyComponent.renderingInfo.value().shape.setTexture(&data::Textures["textures/asteroid.png"]);

    mRegistry.emplace<b2BodyComponent>(entity, bodyComponent);
    return entity;
}

entt::entity State::createPlayer(const sf::Vector2f& position) {
    entt::entity entity = mRegistry.create();

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = b2Vec2(position.x, position.y);

    b2Body* body = mWorld.CreateBody(&bodyDef);

    b2PolygonShape b2shape;
    b2shape.SetAsBox(10.f, 10.f);
    body->CreateFixture(&b2shape, 1.0f/* density */);

    b2BodyComponent bodyComponent {
        .body = body,
        .renderingInfo = b2BodyComponent::RenderingInfo {
            .shape = sf::ConvexShape(4)
        }
    };
    
    // set shape data
    for (int i = 0; i < b2shape.m_count; ++i) {
        bodyComponent.renderingInfo.value().shape.setPoint(i, sf::Vector2f {
            b2shape.m_vertices[i].x,
            b2shape.m_vertices[i].y
        });
    }

    bodyComponent.renderingInfo.value().shape.setFillColor(sf::Color::Transparent);
    bodyComponent.renderingInfo.value().shape.setOutlineColor(sf::Color::White);
    bodyComponent.renderingInfo.value().shape.setOutlineThickness(1.f);

    mRegistry.emplace<b2BodyComponent>(entity, bodyComponent);
    return entity;
}

net::StateUpdate State::getFullStateUpdate() {
    net::StateUpdate SU;
    // SU.updates.reserve(mRegistry.size());
    mRegistry.view<b2BodyComponent>().each([&](auto entity, b2BodyComponent& bodyComponent) {
        SU.updates.emplace_back(
            entity,
            net::StateUpdate::Update::Type::Transform,
            mRegistry
        );
    });
    return SU;
}

};  // namespace game