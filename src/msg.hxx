#pragma once

#include "stdafx.h"

struct BroadcastMessage
{
    std::string type = "broadcast";
    std::string id;
    std::string name;
    std::string host;
    std::string content;
    std::int64_t ts = 0;
};

BroadcastMessage makeBroadcastMessage(std::string_view name, std::string_view content);
std::string serializeMessage(BroadcastMessage const &message);
std::optional<BroadcastMessage> deserializeMessage(std::string_view payload);

class MessageHandler final
{
public:
    using MessageCallback = std::function<void(BroadcastMessage const &message)>;

    explicit MessageHandler(MessageCallback onMessageIn);

    void process(std::string_view payload);
    void remember(std::string_view id);

private:
    static constexpr std::size_t maxRecentIds = 256;

    bool markSeen(std::string_view id);

    MessageCallback onMessage;
    std::deque<std::string> recentIds;
    std::unordered_set<std::string> knownIds;
};
