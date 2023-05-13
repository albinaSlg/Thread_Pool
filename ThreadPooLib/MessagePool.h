#pragma once
#include <map>
#include <string>
#include <chrono>
#include <mutex>
#include <shared_mutex>
#include <optional>

class MessagePool
{
public:
#pragma pack(push, 1)
    struct Message
    {
        enum class Type : __int8
        {
            Info,
            Warning,
            Error,
            Fatal
        };

        using Time_t = std::chrono::time_point<std::chrono::system_clock>;

        std::string message_;
        Type type_;
        Time_t timestamp_ = std::chrono::system_clock::now();
    };
#pragma pack(pop)

public:
    MessagePool() = default;
    MessagePool(MessagePool &&pool);
    MessagePool(const MessagePool &pool);

    void addMessage(const Message& message);
    std::optional<std::vector<MessagePool::Message>> getMessageByType(const Message::Type type) const;

private:
    mutable std::shared_mutex mutex_;
    std::map< Message::Type, std::vector<Message>> messages_;
};

