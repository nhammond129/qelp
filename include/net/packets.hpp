#pragma once

#include <cstdint>
#include <SFML/Network.hpp>

/**
 * Network-related data and functions.
 * 
 * Packet structure:
 * - uint32_t protocolId
 * - Type type
 * - struct data
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
    Ping, Pong
};

struct Header {
    PROTOCOL_ID proto_id = protocolID;
    PacketType type;
};
sf::Packet& operator<<(sf::Packet& packet, const PacketType type) {
    return packet << static_cast<uint32_t>(type);
}
sf::Packet& operator>>(sf::Packet& packet, PacketType& type) {
    uint32_t typeInt;
    packet >> typeInt;
    type = static_cast<PacketType>(typeInt);
    return packet;
}
sf::Packet& operator<<(sf::Packet& packet, const Header& header) {
    return packet << header.proto_id << header.type;
}
sf::Packet& operator>>(sf::Packet& packet, Header& header) {
    return packet >> header.proto_id >> header.type;
}

/* Client to Server */
struct ConnectionRequest {
    static const PacketType packetType = PacketType::ConnectionRequest;
};
/* Server to Client */
struct Challenge {
    static const PacketType packetType = PacketType::Challenge;
    uint32_t client_id;             // server-side ID that client can use to identify itself
    uint32_t challenge_salt;        // server-generated salt
};
/* Client to Server */
struct ChallengeResponse {
    static const PacketType packetType = PacketType::ChallengeResponse;
    uint32_t client_id;             // server-side ID that client can use to identify itself
    uint32_t challenge_salt;        // client-generated salt
    uint32_t challenge_response;    // (server_salt ^ client_salt)
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
    uint32_t ping_id;               // client-generated ID that server echoes back to acknowledge ping
};
/* Server to Client */
struct Pong {
    static const PacketType packetType = PacketType::Pong;
    uint32_t client_id;             // server-side ID that client can use to identify itself
    uint32_t ping_id;               // client-generated ID that server echoes back to acknowledge ping
};

inline sf::Packet& operator<<(sf::Packet& packet, const ConnectionRequest& request) {
    return packet;
}
inline sf::Packet& operator<<(sf::Packet& packet, const Challenge& challenge) {
    return packet << challenge.client_id << challenge.challenge_salt;
}
inline sf::Packet& operator<<(sf::Packet& packet, const ChallengeResponse& response) {
    return packet << response.client_id << response.challenge_salt << response.challenge_response;
}
inline sf::Packet& operator<<(sf::Packet& packet, const ConnectionAccepted& accepted) {
    return packet << accepted.client_id;
}
inline sf::Packet& operator<<(sf::Packet& packet, const Ping& ping) {
    return packet << ping.client_id << ping.ping_id;
}
inline sf::Packet& operator<<(sf::Packet& packet, const Pong& pong) {
    return packet << pong.client_id << pong.ping_id;
}

inline sf::Packet& operator>>(sf::Packet& packet, ConnectionRequest& request) {
    return packet;
}
inline sf::Packet& operator>>(sf::Packet& packet, Challenge& challenge) {
    return packet >> challenge.client_id >> challenge.challenge_salt;
}
inline sf::Packet& operator>>(sf::Packet& packet, ChallengeResponse& response) {
    return packet >> response.client_id >> response.challenge_salt >> response.challenge_response;
}
inline sf::Packet& operator>>(sf::Packet& packet, ConnectionAccepted& accepted) {
    return packet >> accepted.client_id;
}
inline sf::Packet& operator>>(sf::Packet& packet, Ping& ping) {
    return packet >> ping.client_id >> ping.ping_id;
}
inline sf::Packet& operator>>(sf::Packet& packet, Pong& pong) {
    return packet >> pong.client_id >> pong.ping_id;
}

};  // namespace net