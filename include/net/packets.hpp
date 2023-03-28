#pragma once

#include <box2d/box2d.h>
#include <cstdint>
#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>
#include <gamestate.hpp>
#include <SFML/Network.hpp>

/**
 * Network-related data and functions.
 **/
namespace net {

using ClientID = uint32_t;
using PROTOCOL_ID = uint32_t;

static const PROTOCOL_ID protocolID = 0x0000'0000'0000'0001;

enum class PacketType {
    Unknown,
    ConnectionRequest,
    Challenge,
    ChallengeResponse,
    ConnectionAccepted,
    Ping, Pong,
    StateUpdate,
    EntityCreate, EntityDestroy
};

struct Header {
    PROTOCOL_ID proto_id = protocolID;
    PacketType type;
};

/* Client to Server */
struct ConnectionRequest {
    static const PacketType packetType = PacketType::ConnectionRequest;
};
/* Server to Client */
struct Challenge {
    static const PacketType packetType = PacketType::Challenge;
    uint32_t client_id;             // server-side ID that client can use to identify itself
    uint32_t salt;        // server-generated salt
};
/* Client to Server */
struct ChallengeResponse {
    static const PacketType packetType = PacketType::ChallengeResponse;
    uint32_t client_id;             // server-side ID that client can use to identify itself
    uint32_t salt;        // client-generated salt
    uint32_t response;    // (server_salt ^ client_salt)
};
/* Server to Client */
struct ConnectionAccepted {
    static const PacketType packetType = PacketType::ConnectionAccepted;
    uint32_t client_id;             // server-side ID that client can use to identify itself
};
/* Client to Server */
struct Ping {
    static const PacketType packetType = PacketType::Ping;
    uint32_t client_id;             // server-side ID that client can use to identify itself
    uint64_t timestamp_nanosec;     // FUTURE(sometime before 2554 CE): upgrade to uint128_t
    explicit Ping(uint32_t client_id);
};
/* Server to Client */
struct Pong {
    static const PacketType packetType = PacketType::Pong;
    uint32_t client_id;             // server-side ID that client can use to identify itself
    uint64_t timestamp_nanosec;
    Pong() = default;
    explicit Pong(const Ping& ping): client_id(ping.client_id), timestamp_nanosec(ping.timestamp_nanosec) {}
    uint64_t estimate_latency_us() const;
    float estimate_latency_ms() const;
};

struct StateUpdate {
    static const PacketType packetType = PacketType::StateUpdate;
    StateUpdate(): client_id(0), updates({}) {}
    uint32_t client_id;
    struct Update {
        enum class Type {
            Transform
        };
        Update(): entity_id(entt::null), type(Type::Transform), transform() {}
        Update(entt::entity id, Type type, entt::registry& registry): entity_id(id), type(type) {
            switch (type) {
                case Type::Transform: {
                    auto& bodycomponent = registry.get<game::b2BodyComponent>(entity_id);
                    transform = Transform(bodycomponent.body);
                    break;
                }
                default: {
                    break;
                }
            }
        }
        entt::entity entity_id;
        struct Transform {
            Transform(): pos(0.f, 0.f), vel(0.f, 0.f), ang_radians(0.f), ang_vel_radians(0.f) {}
            Transform(b2Body* body): pos(body->GetPosition().x, body->GetPosition().y),
                                     vel(body->GetLinearVelocity().x, body->GetLinearVelocity().y),
                                     ang_radians(body->GetAngle()),
                                     ang_vel_radians(body->GetAngularVelocity()) {}
            sf::Vector2f pos;
            sf::Vector2f vel;
            float ang_radians;
            float ang_vel_radians;
        };
        Type type;
        union {
            Transform transform;
        };
    };
    std::list<Update> updates;
};

/*
struct EntityCreate {
    static const PacketType packetType = PacketType::EntityCreate;
    uint32_t client_id;
    entt::entity entity_id;
    sf::Vector2f pos;
    float ang_radians;
    enum class ShapeType {
        ConvexPoly,
        Circle,
        Rectangle
    };
    ShapeType shape_type;
    union {
        // TODO: use b2Shape instead of sf::Shape lol
        sf::ConvexShape convexpoly;
        sf::CircleShape circle;
        sf::RectangleShape rectangle;
    };
};

struct EntityDestroyBody {
    static const PacketType packetType = PacketType::EntityDestroy;
    uint32_t client_id;
    entt::entity entity_id;
};
*/

// Eyestrain-inducing pile of operator overloaders

inline sf::Packet& operator>>(sf::Packet& packet, sf::Color& color) {
    return packet >> color.r >> color.g >> color.b >> color.a;
}
inline sf::Packet& operator<<(sf::Packet& packet, const sf::Color& color) {
    return packet << color.r << color.g << color.b << color.a;
}

inline sf::Packet& operator<<(sf::Packet& packet, const sf::Vector2f& vec) {
    return packet << vec.x << vec.y;
}
inline sf::Packet& operator>>(sf::Packet& packet, sf::Vector2f& vec) {
    return packet >> vec.x >> vec.y;
}

inline sf::Packet& operator<<(sf::Packet& packet, const sf::CircleShape& shape) {
    return packet << shape.getRadius();
}
inline sf::Packet& operator>>(sf::Packet& packet, sf::CircleShape& shape) {
    float radius;
    packet >> radius;
    shape.setRadius(radius);
    return packet;
}

inline sf::Packet& operator<<(sf::Packet& packet, const sf::RectangleShape& shape) {
    return packet << shape.getSize();
}
inline sf::Packet& operator>>(sf::Packet& packet, sf::RectangleShape& shape) {
    sf::Vector2f size;
    packet >> size;
    shape.setSize(size);
    return packet;
}

inline sf::Packet& operator<<(sf::Packet& packet, const entt::entity& ent_id) {
    return packet << static_cast<uint32_t>(ent_id);
}
inline sf::Packet& operator>>(sf::Packet& packet, entt::entity& ent_id) {
    uint32_t ent_id_int;
    packet >> ent_id_int;
    ent_id = static_cast<entt::entity>(ent_id_int);
    return packet;
}

inline sf::Packet& operator<<(sf::Packet& packet, const StateUpdate::Update& update) {
    packet << update.entity_id << static_cast<uint32_t>(update.type);
    switch (update.type) {
        case StateUpdate::Update::Type::Transform:
            packet << update.transform.pos << update.transform.vel << update.transform.ang_radians << update.transform.ang_vel_radians;
            break;
    }
    return packet;
}
inline sf::Packet& operator>>(sf::Packet& packet, StateUpdate::Update& update) {
    uint32_t update_type_int;
    packet >> update.entity_id >> update_type_int;
    update.type = static_cast<StateUpdate::Update::Type>(update_type_int);
    switch (update.type) {
        case StateUpdate::Update::Type::Transform:
            packet >> update.transform.pos >> update.transform.vel >> update.transform.ang_radians >> update.transform.ang_vel_radians;
            break;
    }
    return packet;
}

inline sf::Packet& operator<<(sf::Packet& packet, const StateUpdate& update) {
    packet << update.client_id << static_cast<uint32_t>(update.updates.size());
    for (const auto& update : update.updates) {
        packet << update;
    }
    return packet;
}
inline sf::Packet& operator>>(sf::Packet& packet, StateUpdate& update) {
    uint32_t update_count;
    packet >> update.client_id >> update_count;
    update.updates.resize(update_count);
    for (auto& update : update.updates) packet >> update;
    return packet;
}

inline sf::Packet& operator<<(sf::Packet& packet, const PacketType type) {
    return packet << static_cast<uint32_t>(type);
}
inline sf::Packet& operator>>(sf::Packet& packet, PacketType& type) {
    uint32_t typeInt;
    packet >> typeInt;
    type = static_cast<PacketType>(typeInt);
    return packet;
}
inline sf::Packet& operator<<(sf::Packet& packet, const Header& header) {
    return packet << header.proto_id << header.type;
}
inline sf::Packet& operator>>(sf::Packet& packet, Header& header) {
    return packet >> header.proto_id >> header.type;
}

inline sf::Packet& operator<<(sf::Packet& packet, const sf::ConvexShape& shape) {
    packet << shape.getPointCount();
    for (size_t i = 0; i < shape.getPointCount(); ++i) {
        packet << shape.getPoint(i);
    }
    return packet;
}
inline sf::Packet& operator>>(sf::Packet& packet, sf::ConvexShape& shape) {
    size_t point_count;
    packet >> point_count;
    shape.setPointCount(point_count);
    for (size_t i = 0; i < point_count; ++i) {
        sf::Vector2f point;
        packet >> point;
        shape.setPoint(i, point);
    }
    return packet;
}

/*
inline sf::Packet& operator<<(sf::Packet& packet, const EntityCreate& entity_create) {
    packet << entity_create.client_id << entity_create.entity_id << entity_create.pos << entity_create.ang_radians << static_cast<uint32_t>(entity_create.shape_type);
    switch (entity_create.shape_type) {
        case EntityCreate::ShapeType::ConvexPoly:
            packet << entity_create.convexpoly;
            break;
        case EntityCreate::ShapeType::Circle:
            packet << entity_create.circle;
            break;
        case EntityCreate::ShapeType::Rectangle:
            packet << entity_create.rectangle;
            break;
    }
    return packet;
}
inline sf::Packet& operator>>(sf::Packet& packet, EntityCreate& body) {
    uint32_t shape_type_int;
    packet >> body.client_id >> body.entity_id >> body.pos >> body.ang_radians >> shape_type_int;
    body.shape_type = static_cast<EntityCreate::ShapeType>(shape_type_int);
    switch (body.shape_type) {
        case EntityCreate::ShapeType::ConvexPoly:
            packet >> body.convexpoly;
            break;
        case EntityCreate::ShapeType::Circle:
            packet >> body.circle;
            break;
        case EntityCreate::ShapeType::Rectangle:
            packet >> body.rectangle;
            break;
    }
    return packet;
}

inline sf::Packet& operator<<(sf::Packet& packet, const EntityDestroyBody& body) {
    packet << body.client_id << body.entity_id;
    return packet;
}
inline sf::Packet& operator>>(sf::Packet& packet, EntityDestroyBody& body) {
    packet >> body.client_id >> body.entity_id;
    return packet;
}
*/

inline sf::Packet& operator<<(sf::Packet& packet, const ConnectionRequest& request) {
    return packet;
}
inline sf::Packet& operator<<(sf::Packet& packet, const Challenge& challenge) {
    return packet << challenge.client_id << challenge.salt;
}
inline sf::Packet& operator<<(sf::Packet& packet, const ChallengeResponse& response) {
    return packet << response.client_id << response.salt << response.response;
}
inline sf::Packet& operator<<(sf::Packet& packet, const ConnectionAccepted& accepted) {
    return packet << accepted.client_id;
}
inline sf::Packet& operator<<(sf::Packet& packet, const Ping& ping) {
    return packet << ping.client_id << ping.timestamp_nanosec;
}
inline sf::Packet& operator<<(sf::Packet& packet, const Pong& pong) {
    return packet << pong.client_id << pong.timestamp_nanosec;
}

inline sf::Packet& operator>>(sf::Packet& packet, ConnectionRequest& request) {
    return packet;
}
inline sf::Packet& operator>>(sf::Packet& packet, Challenge& challenge) {
    return packet >> challenge.client_id >> challenge.salt;
}
inline sf::Packet& operator>>(sf::Packet& packet, ChallengeResponse& response) {
    return packet >> response.client_id >> response.salt >> response.response;
}
inline sf::Packet& operator>>(sf::Packet& packet, ConnectionAccepted& accepted) {
    return packet >> accepted.client_id;
}
inline sf::Packet& operator>>(sf::Packet& packet, Ping& ping) {
    return packet >> ping.client_id >> ping.timestamp_nanosec;
}
inline sf::Packet& operator>>(sf::Packet& packet, Pong& pong) {
    return packet >> pong.client_id >> pong.timestamp_nanosec;
}

};  // namespace net