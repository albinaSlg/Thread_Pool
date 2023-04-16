#pragma once
#include "MultiFuture.h"
#include <queue>
#include <thread>
#include <vector>
#include <deque>
#include <mutex>
#include <functional>
#include <condition_variable>
#include <future>

class ThreadPool {

private:

public:
    ThreadPool(std::size_t threadsNubmer);
    ThreadPool() = delete;
    ~ThreadPool();

    void stop();
    void run();

    template<typename Func, typename ...Args>
    auto submit(Func&& f, Args && ...args);

    template<typename Func, typename ...Args>
    auto parallelize(std::size_t from, std::size_t to, std::size_t threadsNubmer, Args && ...args, Func&& f);

    template <typename Func, typename... Args >
    void pushTask(Func&& f, Args&&... args);

    //MultiFuture multiFuture_;
private:

    std::vector<std::thread> threadsVector_;
    std::queue<std::function<void()>> tasksQueue_;
    //std::deque<std::condition_variable> cvVector_;
    std::deque<std::atomic_bool> endAll_;

    std::mutex mutex_;
    std::size_t threadsNubmer_;
    bool isRun = true;
};

template<typename Func, typename ...Args>
auto ThreadPool::submit(Func&& f, Args && ...args)
{
    using functionType = decltype(f(args...));
    std::shared_ptr<std::packaged_task<functionType()>> ptask = std::make_shared<std::packaged_task<functionType()>>(f);

    std::lock_guard lock(mutex_);
    tasksQueue_.emplace( [ptask, &args...]()
        {
            (*ptask)(std::forward<Args>(args)...);
        }
    );

   // std::future<functionType> future = ptask->get_future();
   // std::shared_future<functionType> sfuture(std::move(future));
    //multiFuture_.pushFuture<functionType>(sfuture);
    return (*ptask).get_future();
}

template<typename Func, typename ...Args>
void ThreadPool::pushTask(Func&& f, Args&& ...args)
{
    std::lock_guard lock(mutex_);
    tasksQueue_.emplace([f, &args...]()
        {
            f(std::forward<Args>(args)...);
        });
}

template<typename Func, typename ...Args>
auto ThreadPool::parallelize(std::size_t from, std::size_t to, std::size_t threadsNubmer, Args && ...args, Func&& f)
{
    //if (threadsNubmer_ < threadsNubmer)
       // return;
    int increase = to / threadsNubmer;
    for (std::size_t i = 0; i < to; i += increase)
    {
        int a = i;
        int b = a + increase;
        pushTask([f](const int a, const int b)
            {
                f(a, b);
            }, a, b);
    }
}
