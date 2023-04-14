#include <config.hpp>
#include <net/gameserver.hpp>
#include <util.hpp>

int main() {
    util::log("Starting server...");
    net::GameServer server(config::SERVER_PORT);
    server.serve_forever();
    return 0;
}
