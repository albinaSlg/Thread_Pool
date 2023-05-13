#include "TaskPool.h"
#include <utility>

bool TaskPool::empty() const
{
	std::shared_lock lock(mutex_);
	return tasksQueue_.empty();
}

std::size_t TaskPool::getSize() const
{
	return tasksQueue_.size();
}

void TaskPool::add(std::function<void()>&& function)
{
	std::lock_guard lock(mutex_);
	tasksQueue_.emplace(std::forward<std::function<void()>>(function));
}

std::function<void()> TaskPool::pop()
{
	std::unique_lock lock(mutex_);
	if (tasksQueue_.empty())
		return std::function<void()>();

	auto task = tasksQueue_.front();
	tasksQueue_.pop();
	lock.unlock();

	return task;
}

void TaskPool::clear()
{
	std::lock_guard lock(mutex_);
	for (std::size_t i = 0; i < tasksQueue_.size(); ++i)
		tasksQueue_.pop();
}
