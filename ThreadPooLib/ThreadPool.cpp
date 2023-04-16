#include "ThreadPool.h"

ThreadPool::ThreadPool(std::size_t threadsNubmer) : threadsNubmer_(threadsNubmer)
{
    threadsVector_.reserve(threadsNubmer_);
    for (std::size_t i = 0; i < threadsNubmer_; ++i) 
    {
        endAll_.emplace_back(false);
        threadsVector_.emplace_back(std::thread([this, i]()
            {
                while (isRun) 
                {
                    std::unique_lock lock(mutex_);
                    while (tasksQueue_.empty() && isRun)
                    {
                        lock.unlock();
                        lock.lock();
                    }

                    if (isRun)
                    {
                        std::function<void()> func = std::move(tasksQueue_.front());
                        tasksQueue_.pop();
                        lock.unlock();
                        func();
                    }
                    
                }
            endAll_[i] = true; 
            endAll_[i].notify_one();
            })).detach();
    }
}

ThreadPool::~ThreadPool()
{
    stop();
}

void ThreadPool::stop()
{
    isRun = false;
    for (auto& end : endAll_)
        end.wait(false);
}

void ThreadPool::run()
{
    isRun = true;
}
