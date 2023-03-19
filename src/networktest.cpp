#include <net/server.hpp>
#include <net/client.hpp>
#include <util.hpp>



int main() {
    // start server thread
    std::thread serverThread([]() {
        net::Server server(0xf00d);
        server.run();
    });

    // start N client threads
    const int N = 64;
    for (int i = 0; i < N; i++) {
        std::thread clientThread([]() {
            net::Client client(sf::IpAddress {127, 0, 0, 1}, 0xf00d);
            client.run();
        });
        clientThread.detach();
    }

    serverThread.join();
    return 0;
}
