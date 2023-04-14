#include <net/gameserver.hpp>
#include <thread>

namespace net {

GameServer::GameServer(const uint32_t port) : Server(port) {
    util::log("Starting game server...");
}

void GameServer::serve_forever() {
    util::log("Serving forever...");

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
        // vomit state updates at connected clients
        StateUpdate stateUpdate = mGameState.getFullStateUpdate();
        for (auto& client : mClients) {
            if (client.second.state == Client::State::Connected) {
                stateUpdate.client_id = client.first;
                sf::Packet statePacket;
                statePacket << Header { .type = PacketType::StateUpdate }
                            << stateUpdate;
                send(statePacket, client.second.address, client.second.port);
            }
        }
        if (!(receive(packet, remoteAddr, remotePort) == sf::Socket::Status::Done)) continue;
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
                /* TODO:
                 *  - send a player object id to the client
                 */

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

void GameServer::serve_forever_nonblocking() {
    util::log("Serving forever (non-blocking)...");
    // create a thread to run serve_forever in
    std::thread t([this]() {
        this->serve_forever();
    });
    t.detach();
}

};  // namespace net