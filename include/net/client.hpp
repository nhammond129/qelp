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
        util::debuglog("Connected! I am client " + std::to_string(mID));

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

            Header header;
            if (!(packet >> header)) continue;
            if (header.proto_id != protocolID) continue;  // wrong protocol. TODO: inform server
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
        packet << protocolID << PacketType::ConnectionRequest;
        auto status = mSocket.send(packet, mServerAddress, mServerPort);
        if (status != sf::Socket::Status::Done) {
            util::log("Failed to send connection request");
            std::exit(1);
        }
        util::debuglog("Sent connection request to " + mServerAddress.toString() + ":" + std::to_string(mServerPort));
    }
    void handleChallenge() {
        util::debuglog("Awaiting challenge...");
        std::optional<Challenge> challenge_opt;
        while(true) {
            challenge_opt = expect<Challenge>();
            if (!challenge_opt) continue;
            util::debuglog("Received challenge (salt: " + std::to_string(challenge_opt.value().challenge_salt) + ")");

            mID = challenge_opt.value().client_id;
            uint32_t server_salt = challenge_opt.value().challenge_salt;
            uint32_t client_salt = std::rand();
            uint32_t challenge_response = server_salt ^ client_salt;

            sf::Packet packet;
            packet
                << Header {
                    .type = PacketType::ChallengeResponse
                }
                << ChallengeResponse {
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
        std::optional<ConnectionAccepted> ack_opt;
        while(true) {
            ack_opt = expect<ConnectionAccepted>();
            if (!ack_opt) continue;
            assert(ack_opt.value().client_id == mID);
            util::debuglog("Connection accepted");
            break;
        }
    }
    void sendPing() {
        sf::Packet packet;
        packet
            << Header { .type = PacketType::Ping }
            << Ping(mID);
        auto status = mSocket.send(packet, mServerAddress, mServerPort);
        if (status != sf::Socket::Status::Done) {
            util::log("Failed to send ping");
            std::exit(1);
        }
    }
    void receivePong() {
        std::optional<Pong> pong_opt;
        while(true) {
            pong_opt = expect<Pong>();
            if (!pong_opt) continue;
            assert(pong_opt.value().client_id == mID);
            util::debuglog("pongping! Client #" + std::to_string(mID) + " [client]");
            break;
        }
    }
    

    ClientID mID;
    sf::UdpSocket mSocket;
    const sf::IpAddress mServerAddress;
    const uint16_t mServerPort;
};

};  // namespace net