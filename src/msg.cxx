#include "msg.hxx"
#include <cstdio>

namespace {
std::string generateUuid() {
    thread_local std::random_device randomDevice;
    thread_local std::mt19937 randomEngine{ randomDevice() };
    thread_local uuids::uuid_random_generator generator{ randomEngine };
    return uuids::to_string(generator());
}

std::int64_t currentTimestampMs() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
}
}

BroadcastMessage makeBroadcastMessage(std::string_view name, std::string_view content) {
    BroadcastMessage message;
    message.id = generateUuid();
    message.name = std::string(name);
    message.host = asio::ip::host_name();
    message.content = std::string(content);
    message.ts = currentTimestampMs();
    return message;
}

std::string serializeMessage(BroadcastMessage const &message) {
    return rfl::json::write(message);
}

std::optional<BroadcastMessage> deserializeMessage(std::string_view payload) {
    auto parsed = rfl::json::read<BroadcastMessage>(std::string(payload));
    if (!parsed) {
        std::println(std::cerr, "Failed to parse message: {}", parsed.error().what());
        return std::nullopt;
    }

    auto message = parsed.value();
    if (message.type != "broadcast" || message.id.empty() || message.content.empty()) {
        return std::nullopt;
    }
    return message;
}

MessageHandler::MessageHandler(MessageCallback onMessageIn)
    : onMessage{ std::move(onMessageIn) } { }

void MessageHandler::process(std::string_view payload) {
    std::println("Received message: {}", payload);
    auto message = deserializeMessage(payload);
    if (!message || !markSeen(message->id)) {
        return;
    }

    if (onMessage) {
        onMessage(*message);
    }
}

void MessageHandler::remember(std::string_view id) {
    static_cast<void>(markSeen(id));
}

bool MessageHandler::markSeen(std::string_view id) {
    if (id.empty()) {
        return false;
    }

    auto [_, inserted] = knownIds.emplace(id);
    if (!inserted) {
        return false;
    }

    recentIds.emplace_back(id);
    if (recentIds.size() > maxRecentIds) {
        knownIds.erase(recentIds.front());
        recentIds.pop_front();
    }
    return true;
}
