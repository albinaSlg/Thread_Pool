#include "ThreadPool.h"

ThreadPool::ThreadPool(std::size_t threadsNubmer) : threadsNubmer_(threadsNubmer)
{
    if (threadsNubmer_ > std::thread::hardware_concurrency())
        messagePool_.addMessage(MessagePool::Message("The number of requested threads exceeds the number of hardware threads",
            MessagePool::Message::Type::Warning));

    threadsVector_.reserve(threadsNubmer_);

    for (std::size_t i = 0; i < threadsNubmer_; ++i) 
    {
        threadsVector_.emplace_back(std::thread([this]()
            {
                while (isRun) 
                {
                    std::unique_lock lock(mutex_);
                    while (tasksPool_.empty() && isRun)
                    {
                        lock.unlock();
                        lock.lock();
                    }
                    isEndCurTask = false;
                    if (isRun)
                    {
                        std::function<void()> func = tasksPool_.pop();
                        lock.unlock();
                        func();
                        isEndCurTask = true;
                    }              
                }
            }));
    }
}

ThreadPool::~ThreadPool()
{
    stop();
}

void ThreadPool::stop()
{
    isRun = false;
    for (auto& thread : threadsVector_)
        if(thread.joinable())
            thread.join();
    
}

void ThreadPool::run()
{
    if(isRun)
        messagePool_.addMessage(MessagePool::Message("Already running",
            MessagePool::Message::Type::Info));

    isRun = true;
}

std::size_t ThreadPool::tasksCount() const
{
    return tasksPool_.getSize();
}

std::size_t ThreadPool::threadsCount() const
{
    return threadsNubmer_;
}

std::vector<MessagePool::Message>
ThreadPool::getMessages(MessagePool::Message::Type type) const
{
    using Type = MessagePool::Message::Type;

    return messagePool_.getMessageByType(type).value_or(
        std::vector<MessagePool::Message>{});
}

void ThreadPool::waitForAllTasks()
{
    while (!(tasksPool_.empty() && isEndCurTask)) {};
}
