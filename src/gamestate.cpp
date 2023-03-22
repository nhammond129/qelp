#include <box2d/box2d.h>
#include <gamestate.hpp>

namespace game {

void b2BodyComponent::draw(sf::RenderTarget& target, sf::RenderStates states) {
    if (!renderingInfo.has_value()) return;
    RenderingInfo& info = renderingInfo.value();

    // update shape data
    info.shape.setPosition(sf::Vector2f {body->GetPosition().x, body->GetPosition().y});
    info.shape.setRotation(sf::radians(body->GetAngle()));

    target.draw(info.shape, states);
}

State::State(): mWorld(b2Vec2(0.f, 0.f)) {
    
}

void State::update(const sf::Time& dt) {
    const int32_t CVelocityIterations = 4;
    const int32_t CPositionIterations = 4;
    mWorld.Step(dt.asSeconds(), CVelocityIterations, CPositionIterations);
}

entt::entity State::createPlayer(const sf::Vector2f& position) {
    entt::entity entity = mRegistry.create();

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = b2Vec2(position.x, position.y);

    b2Body* body = mWorld.CreateBody(&bodyDef);
    // body->SetUserData(static_cast<void*>(entity));

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

};  // namespace game