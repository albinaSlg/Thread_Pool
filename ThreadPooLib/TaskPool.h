#pragma once
#include <queue>
#include <functional>
#include <shared_mutex>

class TaskPool final
{
public:
	TaskPool() = default;
	~TaskPool() = default;

	_NODISCARD bool empty() const;
	_NODISCARD std::size_t getSize() const;

	void add(std::function<void()>&& function);
	_NODISCARD std::function<void()> pop();
	void clear();

private:
	mutable std::shared_mutex mutex_;
	std::queue<std::function<void()>> tasksQueue_;
};