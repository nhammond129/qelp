#pragma once

#include <iostream>
#include <net/packets.hpp>
#include <SFML/Network.hpp>
#include <util.hpp>

namespace net {

class Client {
public:
    Client(sf::IpAddress serverAddress, uint16_t serverPort) : mServerAddress(serverAddress), mServerPort(serverPort) {
        auto status = mSocket.bind(sf::Socket::AnyPort);
        if (status != sf::Socket::Status::Done) {
            util::log("Failed to bind socket");
            std::exit(1);
        }
    }
    void run() {
        requestConnect();
        handleChallenge();
        awaitConnectionAck();
        util::log("Connected! I am client " + std::to_string(mID));

        // send pings every 1 second lol

        while(true) {
            sendPing();
            receivePong();
            sf::sleep(sf::seconds(1));
        }
    }
private:
    template<typename T>
    std::optional<T> expect(uint32_t timeout_seconds = 30) {
        T t;
        sf::Packet packet;
        std::optional<sf::IpAddress> remoteAddr = std::nullopt;
        unsigned short remotePort;

        while(true) {  // TODO: timeout
            if (!(mSocket.receive(packet, remoteAddr, remotePort) == sf::Socket::Status::Done)) continue;
            if (remoteAddr.value() != mServerAddress || remotePort != mServerPort) {
                util::log("Received packet from " + remoteAddr.value().toString() + ":" + std::to_string(remotePort) + " but expected " + mServerAddress.toString() + ":" + std::to_string(mServerPort));
                continue;
            }

            net::Header header;
            if (!(packet >> header)) continue;
            if (header.proto_id != net::protocolID) continue;  // wrong protocol. TODO: inform server
            if (header.type != T::packetType) {
                util::log("Received packet of type " + std::to_string((int)header.type) + " but expected " + std::to_string((int)T::packetType));
                continue;  // not what we want
            }
            if (!(packet >> t)) continue;  // packet cannot be extracted
            break;  // packet has now been decoded into `t`
        }
        return t;
    }
    void requestConnect() {
        sf::Packet packet;
        packet << net::protocolID << net::PacketType::ConnectionRequest;
        auto status = mSocket.send(packet, mServerAddress, mServerPort);
        if (status != sf::Socket::Status::Done) {
            util::log("Failed to send connection request");
            std::exit(1);
        }
        util::log("Sent connection request to " + mServerAddress.toString() + ":" + std::to_string(mServerPort));
    }
    void handleChallenge() {
        util::log("Awaiting challenge...");
        std::optional<net::Challenge> challenge_opt;
        while(true) {
            challenge_opt = expect<net::Challenge>();
            if (!challenge_opt) continue;
            util::log("Received challenge (salt: " + std::to_string(challenge_opt.value().challenge_salt) + ")");

            mID = challenge_opt.value().client_id;
            uint32_t server_salt = challenge_opt.value().challenge_salt;
            uint32_t client_salt = std::rand();
            uint32_t challenge_response = server_salt ^ client_salt;

            sf::Packet packet;
            packet
                << net::Header {
                    .type = net::PacketType::ChallengeResponse
                }
                << net::ChallengeResponse {
                    .client_id = mID,
                    .challenge_salt = client_salt,
                    .challenge_response = challenge_response
                };
            auto status = mSocket.send(packet, mServerAddress, mServerPort);
            if (status != sf::Socket::Status::Done) {
                util::log("Failed to send challenge response");
                std::exit(1);
            }
            break;
        }
    }
    void awaitConnectionAck() {
        std::optional<net::ConnectionAccepted> ack_opt;
        while(true) {
            ack_opt = expect<net::ConnectionAccepted>();
            if (!ack_opt) continue;
            assert(ack_opt.value().client_id == mID);
            util::log("Connection accepted");
            break;
        }
    }
    void sendPing() {
        uint32_t value = (uint32_t)std::rand();
        sf::Packet packet;
        packet
            << net::Header { .type = net::PacketType::Ping }
            << net::Ping { .client_id = mID, .ping_id = value };
        auto status = mSocket.send(packet, mServerAddress, mServerPort);
        if (status != sf::Socket::Status::Done) {
            util::log("Failed to send ping");
            std::exit(1);
        }
    }
    void receivePong() {
        std::optional<net::Pong> pong_opt;
        while(true) {
            pong_opt = expect<net::Pong>();
            if (!pong_opt) continue;
            assert(pong_opt.value().client_id == mID);
            util::log("pongping! Client #" + std::to_string(mID));
            break;
        }
    }
    

    net::ClientID mID;
    sf::UdpSocket mSocket;
    const sf::IpAddress mServerAddress;
    const uint16_t mServerPort;
};

};  // namespace net