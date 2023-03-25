#include <net/server.hpp>
#include <net/client.hpp>
#include <util.hpp>



int main() {
    // dedicated server
    net::Server server(0xf00d);
    server.run();
    return 0;
}
