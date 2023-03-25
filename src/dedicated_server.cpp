#include <net/server.hpp>
#include <util.hpp>

int main() {
    util::log("Starting server...");
    net::Server server(0xf00d);
    server.serve();
    return 0;
}
