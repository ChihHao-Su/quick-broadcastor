#pragma once

#include "stdafx.h"

class NetworkFunctionality final
{
public:
    using ReceiveCallback = std::function<void(std::string_view content)>;

    explicit NetworkFunctionality(ReceiveCallback onReceiveIn);
    ~NetworkFunctionality();

    NetworkFunctionality(NetworkFunctionality const &) = delete;
    NetworkFunctionality &operator=(NetworkFunctionality const &) = delete;
    NetworkFunctionality(NetworkFunctionality &&) = delete;
    NetworkFunctionality &operator=(NetworkFunctionality &&) = delete;

    void broadcast(std::string payload);

private:
    using UdpSocket = asio::ip::udp::socket;

    static void pollIo(void *userdata);

    asio::awaitable<void> broadcastLoop(std::string payload);
    asio::awaitable<void> receiveLoop();

    asio::io_context ioContext;
    UdpSocket socket;
    ReceiveCallback onReceive;
};
