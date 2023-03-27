#include <net/server.hpp>
#include <optional>

namespace net {

Server::Server(const uint32_t port) : mPort(port), _next_id(0) {
    util::log("Server started at port " + std::to_string(port));
}

void Server::serve_forever() {
    auto status = mSocket.bind(mPort);
    if (status != sf::Socket::Status::Done) {
        util::log("Failed to bind socket");
        std::exit(1);
    }
    mSocket.setBlocking(false);  // necessary?

    sf::Packet packet;
    unsigned short remotePort;
    std::optional<sf::IpAddress> remoteAddr = std::nullopt;
    while (true) {
        // check clients for timeouts
        const auto now = util::Clock::now();
        for (auto it = mClients.begin(); it != mClients.end();) {
            if ((now - it->second.lastPacketTime) > timeout) {
                util::log("Client " + it->second.address.toString() + ":" + std::to_string(it->second.port) + " timed out");
                it = mClients.erase(it);
            } else {
                ++it;
            }
        }
        if (!(mSocket.receive(packet, remoteAddr, remotePort) == sf::Socket::Status::Done)) continue;
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
                uint32_t salt = (uint32_t)std::rand();
                ClientID cid = add(remoteAddr.value(), remotePort, salt);
                net::Challenge challenge {
                    .client_id = cid,
                    .salt = salt
                };
                sf::Packet challengePacket;
                challengePacket << Header { .type = net::PacketType::Challenge } << challenge;
                
                if (!send(challengePacket, remoteAddr.value(), remotePort)) {
                    util::log("Failed to send challenge packet");
                    break;
                }
                util::debuglog("Sent challenge to " + remoteAddr.value().toString() + ":" + std::to_string(remotePort) + " (salt: " + std::to_string(challenge.salt) + ")");

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
                client.client_salt = response.salt;
                client.response = response.response;

                uint32_t expected_response = client.challenge_issued ^ response.salt;
                if (expected_response != response.response) {
                    // challenge failed
                    util::log("Challenge  issued: " + std::to_string(client.challenge_issued));
                    util::log("Client's     salt: " + std::to_string(response.salt));
                    util::log("Actual   response: " + std::to_string(response.response));
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
                Ping ping(NULL);
                if (!(packet >> ping) || !hasClient(ping.client_id)) break;
                Client& client = lookupClient(ping.client_id);
                if (client.address != remoteAddr.value() || client.port != remotePort) break;  // wrong client!
                client.lastPacketTime = util::Clock::now();

                sf::Packet pongPacket;
                pongPacket << Header { .type = PacketType::Pong } << Pong(ping);
                send(pongPacket, client);
                util::debuglog("pingpong! Client #" + std::to_string(ping.client_id) + " [server]");
                break;
            }
        }
    }
}

inline bool Server::send(sf::Packet& packet, sf::IpAddress addr, uint32_t port)  {
    auto status = mSocket.send(packet, addr, port);
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

inline bool Server::send(sf::Packet& packet, const Server::Client& client) {
    return send(packet, client.address, client.port);
}

inline bool Server::send(sf::Packet& packet, ClientID id) {
    if (!hasClient(id)) return false;
    Client& client = lookupClient(id);
    return send(packet, client);
}

inline Server::Client& Server::lookupClient(const ClientID id) {
    assert(hasClient(id));
    return mClients[id];
}

inline bool Server::hasClient(ClientID id) const  {
    return mClients.find(id) != mClients.end();
}

inline ClientID Server::add(sf::IpAddress addr, uint32_t port, uint32_t challenge_issued) {
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
        .address = addr,
        .port = port,
        .challenge_issued = challenge_issued,
        .lastPacketTime = util::Clock::now(),
        .state = Client::State::Connecting
    };
    return id;
}

inline void Server::remove(ClientID id) {
    mClients.erase(id);
    mFreeIDs.push_front(id);
}

};  // namespace net