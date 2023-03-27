#pragma once

#include <iostream>
#include <net/packets.hpp>
#include <SFML/Network.hpp>
#include <util.hpp>

namespace net {

class Client {
public:
    enum class State {
        Disconnected,
        AwaitingChallenge,
        AwaitingConnectionAck,
        Connected
    };
    Client(const sf::IpAddress address, const uint16_t serverPort);
    void run();
private:
    void consumePacket();
    void connect();

    template<typename T>
    [[nodiscard]] bool send(const T&& packable);

    template<typename T>
    std::optional<T> expect(uint32_t timeout_seconds = 30);
    void handleChallenge();
    void awaitConnectionAck();
    void sendPing();
    void receivePong();

    ClientID mID;
    sf::UdpSocket mSocket;
    const sf::IpAddress mServerAddress;
    const uint16_t mServerPort;
    State mState = State::Disconnected;
};

};  // namespace net