#include <net/server.hpp>

namespace net {

class GameServer : protected Server {
public:
    GameServer(const uint32_t port);
    void serve_forever() override;
    void serve_forever_nonblocking();

protected:
    game::State mGameState;
};

};  // namespace net