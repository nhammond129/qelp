#pragma once

#include <net/packets.hpp>
#include <optional>
#include <SFML/Network.hpp>
#include <util.hpp>

// https://gafferongames.com/post/client_server_connection/

namespace net {

class Server {
    struct Client {
        enum class State {
            Connecting,
            Connected,
            Dead
        };
        sf::IpAddress address = {127, 0, 0, 1};
        uint32_t port;
        uint32_t challenge_issued;
        uint32_t client_salt;
        uint32_t challenge_response;
        util::Time lastPacketTime;
        State state;
    };
public:
    Server(uint32_t port) : mPort(port) {
        _next_id = 0;
    }

    void run() {
        auto status = socket.bind(mPort);
        if (status != sf::Socket::Status::Done) {
            util::log("Failed to bind socket");
            std::exit(1);
        }
        socket.setBlocking(false);

        sf::Packet packet;
        unsigned short remotePort;
        std::optional<sf::IpAddress> remoteAddr = std::nullopt;
        while (true) {
            if (!(socket.receive(packet, remoteAddr, remotePort) == sf::Socket::Status::Done)) continue;
            net::Header header;
            if (!(packet >> header)) continue;
            if (header.proto_id != net::protocolID) continue;  // wrong protocol. TODO: inform client
            switch (header.type) {
                case net::PacketType::ConnectionRequest: {
                    util::log("Connection request from " + remoteAddr.value().toString() + ":" + std::to_string(remotePort));
                    if (mClients.size() >= maxClients) {
                        // server is full.
                        // TODO: inform client
                        break;
                    }
                    uint32_t challenge_salt = (uint32_t)std::rand();
                    ClientID cid = add(remoteAddr.value(), remotePort, challenge_salt);
                    net::Challenge challenge {
                        .client_id = cid,
                        .challenge_salt = challenge_salt
                    };
                    sf::Packet challengePacket;
                    challengePacket << Header { .type = net::PacketType::Challenge } << challenge;
                    
                    if (!send(challengePacket, remoteAddr.value(), remotePort)) {
                        util::log("Failed to send challenge packet");
                        break;
                    }
                    util::debuglog("Sent challenge to " + remoteAddr.value().toString() + ":" + std::to_string(remotePort) + " (salt: " + std::to_string(challenge.challenge_salt) + ")");

                    break;
                }
                case PacketType::ChallengeResponse: {
                    util::debuglog("Challenge response from " + remoteAddr.value().toString() + ":" + std::to_string(remotePort));
                    ChallengeResponse response;
                    
                    if (!(packet >> response)) break;
                    util::debuglog("Client ID: " + std::to_string(response.client_id));
                    if (!hasClient(response.client_id)) break;
                    Client& client = lookupClient(response.client_id);
                    if (client.address != remoteAddr.value() || client.port != remotePort) break;  // wrong client!
                    client.lastPacketTime = util::Clock::now();
                    client.client_salt = response.challenge_salt;
                    client.challenge_response = response.challenge_response;

                    uint32_t expected_response = client.challenge_issued ^ response.challenge_salt;
                    if (expected_response != response.challenge_response) {
                        // challenge failed
                        util::log("Challenge  issued: " + std::to_string(client.challenge_issued));
                        util::log("Client's     salt: " + std::to_string(response.challenge_salt));
                        util::log("Actual   response: " + std::to_string(response.challenge_response));
                        util::log("Expected response: " + std::to_string(expected_response));
                        remove(response.client_id);
                    }
                    client.state = Client::State::Connected;

                    // report accepted
                    sf::Packet acceptedPacket;
                    acceptedPacket << Header { .type = PacketType::ConnectionAccepted } << ConnectionAccepted {
                        .client_id = response.client_id
                    };
                    send(acceptedPacket, client);
                    break;
                }
                case PacketType::Ping: {
                    Ping ping;
                    if (!(packet >> ping) || !hasClient(ping.client_id)) break;
                    Client& client = lookupClient(ping.client_id);
                    if (client.address != remoteAddr.value() || client.port != remotePort) break;  // wrong client!
                    client.lastPacketTime = util::Clock::now();

                    sf::Packet pongPacket;
                    pongPacket << Header { .type = PacketType::Pong } << Pong {
                        .client_id = ping.client_id,
                        .ping_id = ping.ping_id
                    };
                    send(pongPacket, client);
                    util::debuglog("pingpong! Client #" + std::to_string(ping.client_id) + " [server]");
                    break;
                }
            }
        }
    }
private:
    inline bool send(sf::Packet& packet, sf::IpAddress addr, uint32_t port) {
        auto status = socket.send(packet, addr, port);
        if (!(status == sf::Socket::Status::Done)) {
            util::log(
                "Failed to send challenge to client (" + \
                    addr.toString() + ":" + \
                    std::to_string(port) + \
                    ")" + \
                " sf::Socket::Status = " + \
                    std::to_string((size_t)status)
            );
            return false;
        }
        return true;
    }
    inline bool send(sf::Packet& packet, const Client& client) {
        return send(packet, client.address, client.port);
    }
    inline bool send(sf::Packet& packet, ClientID id) {
        if (!hasClient(id)) return false;
        Client& client = lookupClient(id);
        return send(packet, client);
    }
    inline Client& lookupClient(const ClientID id) {
        return mClients[id];
    }
    inline bool hasClient(ClientID id) const {
        return mClients.find(id) != mClients.end();
    }
    inline ClientID add(sf::IpAddress address, uint32_t port, uint32_t challenge_issued) {
        if (mClients.size() >= maxClients) {
            throw std::runtime_error("Server is full");
        }
        ClientID id;
        if (mFreeIDs.empty()) {
            id = _next_id++;
        } else {
            id = mFreeIDs.front();
            mFreeIDs.pop_front();
        }
        mClients[id] = {
            .address = address,
            .port = port,
            .challenge_issued = challenge_issued,
            .lastPacketTime = util::Clock::now(),
            .state = Client::State::Connecting
        };
        return id;
    }
    inline void remove(ClientID id) {
        mClients.erase(id);
        mFreeIDs.push_front(id);
    }

    sf::UdpSocket socket;
    const uint32_t maxClients = 0xFFFF;;
    const unsigned short mPort;
    ClientID _next_id;
    std::unordered_map<ClientID, Client> mClients;
    std::forward_list<ClientID> mFreeIDs;
};

}  // namespace net
