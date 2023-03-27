#include <net/client.hpp>

namespace net {

Client::Client(const sf::IpAddress address, const uint16_t port): mServerAddress(address), mServerPort(port) {
    auto status = mSocket.bind(sf::Socket::AnyPort);
    if (status != sf::Socket::Status::Done) {
        util::log("Failed to bind socket");
        std::exit(1);
    }
}

void Client::run() {
    while(true) {
        consumePacket();

        switch(mState) {
            case State::Disconnected: {
                connect();
                mState = State::AwaitingChallenge;
                break;
            }
            case State::Connected: {
                sendPing();
                sf::sleep(sf::seconds(1));
                break;
            }
            default: {
            }
        }
    }
}

void Client::consumePacket() {
    sf::Packet packet;
    std::optional<sf::IpAddress> remoteAddr = std::nullopt;
    uint16_t remotePort;

    if (!(mSocket.receive(packet, remoteAddr, remotePort) == sf::Socket::Status::Done)) {
        return;  // nothing to process yet
    }

    if (remoteAddr.value() != mServerAddress || remotePort != mServerPort) {
        util::debuglog("Received packet from " + remoteAddr.value().toString() + ":" + std::to_string(remotePort) + " but expected " + mServerAddress.toString() + ":" + std::to_string(mServerPort));
        return;
    }

    Header header;
    if (!(packet >> header)) {
        util::debuglog("Failed to extract header");
        return;
    }
    if (header.proto_id != protocolID) {
        util::debuglog("Received packet with wrong protocol ID");
        return;  // wrong protocol. TODO: inform server
    }
    switch(header.type) {
        case PacketType::Unknown: {
            util::debuglog("Received unknown packet");
            break;
        }
        case PacketType::Challenge: {
            if (mState != State::AwaitingChallenge) {
                util::debuglog("Client received challenge but was not expecting one");
                break;
            }

            Challenge challenge;
            if (!(packet >> challenge)) {
                util::debuglog("Failed to extract challenge");
                break;
            }
            util::debuglog("Received challenge (salt: " + std::to_string(challenge.salt) + ")");
            
            mID = challenge.client_id;
            uint32_t server_salt = challenge.salt;
            uint32_t client_salt = std::rand();
            uint32_t response = server_salt ^ client_salt;

            bool success = send<ChallengeResponse>(ChallengeResponse {
                .client_id = mID,
                .salt = client_salt,
                .response = response
            });

            if (success) {
                util::debuglog("Sent challenge response");
                mState = State::AwaitingConnectionAck;
            } else {
                util::debuglog("Failed to send challenge response!");
                mState = State::Disconnected;
            }

            break;
        }
        case PacketType::ConnectionAccepted: {
            if (mState != State::AwaitingConnectionAck) {
                util::debuglog("Client received connection ack but was not expecting one!");
                break;
            }

            ConnectionAccepted ack;
            if (!(packet >> ack)) {
                util::debuglog("Failed to extract connection ack");
                break;
            }
            util::debuglog("Received connection ack");
            assert(ack.client_id == mID);
            mState = State::Connected;
            break;
        }
        case PacketType::Pong: {
            Pong pong;
            if (!(packet >> pong)) {
                util::debuglog("Failed to extract pong");
                break;
            }

            util::log("Latency: " + std::to_string(pong.estimate_latency_ms()) + "ms");

            util::debuglog("Received pong from " + std::to_string(pong.client_id));
            break;
        }
        default: {
            util::debuglog("Received unhandled packet of enum type #" + std::to_string((uint32_t)header.type));
            break;
        }
    }
}

template<typename T>
[[nodiscard("Send function should be checked for success")]]
bool Client::send(const T&& packable) {
    sf::Packet packet;
    packet << Header { .type = T::packetType }
           << packable;
    auto status = mSocket.send(packet, mServerAddress, mServerPort);
    if (status != sf::Socket::Status::Done) {
        util::log("panic: Failed to send packet of type '" + std::string(typeid(T).name()) + "'");
        std::exit(1);
    }
    return true;
}

void Client::connect() {
    sf::Packet packet;
    auto success = send<ConnectionRequest>(ConnectionRequest {});
    if (!success) {
        util::log("Failed to send connection request");
        mState = State::Disconnected;
    }
    util::debuglog("Sent connection request to " + mServerAddress.toString() + ":" + std::to_string(mServerPort));
}

void Client::sendPing() {
    bool success = send<Ping>(Ping(mID));
    if (!success) {
        util::log("Failed to send ping!");
    }
}

};  // namespace net