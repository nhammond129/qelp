#include <config.hpp>
#include <net/server.hpp>
#include <util.hpp>

int main() {
    util::log("Starting server...");
    net::Server server(config::SERVER_PORT);
    server.serve_forever();
    return 0;
}
