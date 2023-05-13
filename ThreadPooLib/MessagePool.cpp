#include "MessagePool.h"

MessagePool::MessagePool(MessagePool &&pool) 
{ 
	messages_ = std::move(pool.messages_);
}

MessagePool::MessagePool(const MessagePool &pool) 
{
	messages_ = pool.messages_;
}

void MessagePool::addMessage(const Message &message)
{
	std::lock_guard lock(mutex_);

	auto keyIterator = messages_.find(message.type_);

	if (keyIterator != messages_.end())
		keyIterator->second.emplace_back(message);
	else
		messages_.emplace(message.type_, std::vector<Message>{ message });
}

std::optional<std::vector<MessagePool::Message>> MessagePool::getMessageByType(const Message::Type type) const
{
	std::shared_lock lock(mutex_);

	auto keyIterator = messages_.find(type);

	if (keyIterator != messages_.end())
		return keyIterator->second;

	return {};
}
