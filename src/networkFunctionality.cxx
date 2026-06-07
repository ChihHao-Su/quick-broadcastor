#include "networkFunctionality.hxx"

using namespace std::chrono_literals;

static constexpr unsigned short port = 54321;
static constexpr double pollInterval = 0.02;
static constexpr int resendCount = 3;
static constexpr auto resendInterval = 200ms;

NetworkFunctionality::NetworkFunctionality(ReceiveCallback onReceiveIn)
    : ioContext{},
      socket{ ioContext },
      onReceive{ std::move(onReceiveIn) } {
    asio::ip::udp::endpoint endpoint{ asio::ip::udp::v4(), port };
    socket.open(endpoint.protocol());
    socket.set_option(asio::socket_base::reuse_address{ true });
    socket.set_option(asio::socket_base::broadcast{ true });
    socket.bind(endpoint);

    asio::co_spawn(ioContext, receiveLoop(), asio::detached);
    Fl::add_timeout(pollInterval, pollIo, this);
}

NetworkFunctionality::~NetworkFunctionality() {
    Fl::remove_timeout(pollIo, this);
    std::error_code ignored;
    auto _ = socket.close(ignored);
    ioContext.poll();
}

void NetworkFunctionality::pollIo(void *userdata) {
    auto &functionality = UNWRAP(static_cast<NetworkFunctionality *>(userdata));
    functionality.ioContext.poll();
    Fl::repeat_timeout(pollInterval, pollIo, userdata);
}

void NetworkFunctionality::broadcast(std::string payload) {
    asio::co_spawn(ioContext, broadcastLoop(std::move(payload)), asio::detached);
}

asio::awaitable<void> NetworkFunctionality::broadcastLoop(std::string payload) {
    auto endpoint = asio::ip::udp::endpoint{ asio::ip::address_v4::broadcast(), port };

    for (int index = 0; index < resendCount; ++index) {
        std::error_code error;
        co_await socket.async_send_to(
            asio::buffer(payload),
            endpoint,
            //asio::redirect_error(asio::use_awaitable, error)
            asio::use_awaitable
        );

        /*if (error == asio::error::operation_aborted) {
            co_return;
        }
        if (error) {
            continue;
        }
        if (index + 1 >= resendCount) {
            continue;
        }*/

        co_await asio::steady_timer{ socket.get_executor(), resendInterval }
            .async_wait(asio::use_awaitable);
        // if (error == asio::error::operation_aborted) {
        //     co_return;
        // }
    }
}

asio::awaitable<void> NetworkFunctionality::receiveLoop() {
    std::array<char, 2048> buffer {};

    for (;;) {
        asio::ip::udp::endpoint sender;
        std::error_code error;
        auto length = co_await socket.async_receive_from(
            asio::buffer(buffer),
            sender,
            asio::redirect_error(asio::use_awaitable, error));

        if (error == asio::error::operation_aborted) {
            co_return;
        }
        if (error) {
            continue;
        }

        if (onReceive) {
            onReceive(std::string_view{ buffer.data(), length });
        }
    }
}
