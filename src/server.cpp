#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <util.hpp>

using namespace std::literals;

// https://gafferongames.com/post/client_server_connection/

class Server {
public:
    int findFreeClientIndex() const {
        for (int i = 0; i < MaxClients; ++i) {
            if (!mClientConnected[i]) return i;
        }
        return -1;
    }
    int findClientIndex(sf::IpAddress address) const {
        for (int i = 0; i < MaxClients; ++i) {
            if (mClientAddress[i] == address) return i;
        }
        return -1;
    }
    bool isClientConnected(sf::IpAddress address) const {
        return findClientIndex(address) != -1;
    }
    void addClient(sf::IpAddress address) {
        int index = findFreeClientIndex();
        if (index == -1) {
            util::log("no free client slots");
            return;
        }
        mClientConnected[index] = true;
        mClientAddress[index] = address;
        ++mConnectedClients;
    }
private:
    static const int MaxClients = 64;  // tmp
    int mConnectedClients;
    bool mClientConnected[MaxClients];
    sf::IpAddress mClientAddress[MaxClients];
};


int main() {
    util::log("aaa");
    return 0;
}
