#pragma once

#include <net/packets.hpp>
#include <gamestate.hpp>
#include <SFML/Network.hpp>
#include <util.hpp>

// https://gafferongames.com/post/client_server_connection/

namespace net {

class Server {
protected:
    struct Client {
        enum class State {
            Connecting,
            Connected,
            TimedOut
        };
        sf::IpAddress address = sf::IpAddress::Any;
        uint32_t port;
        uint32_t challenge_issued;
        uint32_t client_salt;
        uint32_t response;
        util::Time lastPacketTime;
        State state;
    };
public:
    Server(const uint32_t port);
    virtual void serve_forever();

protected:
    inline bool send(sf::Packet& packet, sf::IpAddress addr, uint32_t port);
    inline bool send(sf::Packet& packet, const Client& client);
    inline bool send(sf::Packet& packet, ClientID id);
    sf::UdpSocket::Status receive(sf::Packet& packet, std::optional<sf::IpAddress>& addr, unsigned short& port);
    inline Client& lookupClient(const ClientID id);
    inline bool hasClient(ClientID id) const;
    inline ClientID add(sf::IpAddress address, uint32_t port, uint32_t challenge_issued);
    inline void remove(ClientID id);

    std::unordered_map<ClientID, Client> mClients;
    const uint32_t maxClients = 0xFFFF;;
    const unsigned short mPort;
    inline static const util::Duration timeout = util::Duration {30.f};
private:
    sf::UdpSocket mSocket;
    ClientID _next_id = 0;
    std::forward_list<ClientID> mFreeIDs;
};

}  // namespace net
