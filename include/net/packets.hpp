#pragma once

#include <cstdint>
#include <entt/entt.hpp>
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
    StateUpdate
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
    uint32_t client_id;
    struct Update {
        entt::entity entity_id;
        enum class Type {
            Transform
        };
        struct Transform {
            sf::Vector2f pos;
            sf::Vector2f vel;
            float ang_radians;
            float ang_vel_radians;
        };
        Type update_type;
        union {
            Transform transform;
        };
    };
    std::vector<Update> updates;
};

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
    packet << update.entity_id << static_cast<uint32_t>(update.update_type);
    switch (update.update_type) {
        case StateUpdate::Update::Type::Transform:
            packet << update.transform.pos << update.transform.vel << update.transform.ang_radians << update.transform.ang_vel_radians;
            break;
    }
    return packet;
}
inline sf::Packet& operator>>(sf::Packet& packet, StateUpdate::Update& update) {
    uint32_t update_type_int;
    packet >> update.entity_id >> update_type_int;
    update.update_type = static_cast<StateUpdate::Update::Type>(update_type_int);
    switch (update.update_type) {
        case StateUpdate::Update::Type::Transform:
            packet >> update.transform.pos >> update.transform.vel >> update.transform.ang_radians >> update.transform.ang_vel_radians;
            break;
    }
    return packet;
}

inline sf::Packet& operator<<(sf::Packet& packet, const sf::Vector2f& vec) {
    return packet << vec.x << vec.y;
}
inline sf::Packet& operator>>(sf::Packet& packet, sf::Vector2f& vec) {
    return packet >> vec.x >> vec.y;
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