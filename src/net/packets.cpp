#include <net/packets.hpp>

namespace {

template<typename Rep, typename Period>
std::chrono::duration<Rep, Period> duration_from_ns(
        uint64_t start_ns,
        uint64_t end_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::high_resolution_clock::now().time_since_epoch()
            ).count()) {
    return std::chrono::duration_cast<std::chrono::duration<Rep, Period>>(
        std::chrono::nanoseconds(end_ns) - std::chrono::nanoseconds(start_ns)
    );
}

};  // namespace anonymous

namespace net {

Ping::Ping(uint32_t client_id): client_id(client_id) {
    // goodness gracious that's a lot of characters for a simple assignment
    timestamp_nanosec = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()
    ).count();
}

uint64_t Pong::estimate_latency_us() const {
    // distance between timestamps in microseconds, halved (round trip)
    auto now = std::chrono::high_resolution_clock::now();
    return duration_from_ns<uint64_t, std::micro>(timestamp_nanosec).count() / 2;
}

float Pong::estimate_latency_ms() const {
    // distance between timestamps in milliseconds, halved (round trip)
    return duration_from_ns<float, std::milli>(timestamp_nanosec).count() / 2.f;
}

};  // namespace net