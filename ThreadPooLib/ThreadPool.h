#pragma once
#include "MultiFuture.h"
#include "TaskPool.h"
#include "MessagePool.h"
#include <queue>
#include <thread>
#include <vector>
#include <deque>
#include <mutex>
#include <functional>
#include <condition_variable>
#include <future>
#include <Windows.h>
#include <format>

class ThreadPool 
{

public:
    ThreadPool(std::size_t threadsNubmer = std::thread::hardware_concurrency());
    ~ThreadPool();

    void stop();
    void run();
    void waitForAllTasks();

    _NODISCARD std::size_t tasksCount() const;
    _NODISCARD std::size_t threadsCount() const;
    _NODISCARD std::vector<MessagePool::Message>
        getMessages(MessagePool::Message::Type type) const;

    template<typename Func, typename ...Args>
    auto submit(Func&& f, Args && ...args);

    template<typename Func, typename ...Args>
    auto parallelize(std::size_t from, std::size_t to, std::size_t blockCount, Func&& f, Args && ...args);

    template <typename Func, typename... Args >
    void pushTask(Func&& f, Args&&... args);

private:
    std::vector<std::thread> threadsVector_;
    TaskPool tasksPool_;
    MessagePool messagePool_;

    std::mutex mutex_;
    std::size_t threadsNubmer_;
    bool isRun = true;
    bool isEndCurTask = false;
};

template<typename Func, typename ...Args>
auto ThreadPool::submit(Func&& f, Args && ...args)
{
    using functionType = decltype(f(args...));
    std::shared_ptr<std::packaged_task<functionType()>> ptask = std::make_shared<std::packaged_task<functionType()>>(f);

    tasksPool_.add([ptask, &args...]()
        {
            try
            {
                std::invoke(*ptask, std::forward<Args>(args)...);
                //(*ptask)(std::forward<Args>(args)...);
            }
            catch (std::runtime_error error)
            {
                MessageBoxA(NULL, error.what(), "Runtime error", MB_OK);
            }
            catch (std::exception error)
            {
                MessageBoxA(NULL, error.what(), "Exception", MB_OK);
            }
            catch (...)
            {
                MessageBoxA(NULL, "Undefined exception", "Exception", MB_OK);
            }
        }
    );

    return (*ptask).get_future();
}

template<typename Func, typename ...Args>
void ThreadPool::pushTask(Func&& f, Args&& ...args)
{
    tasksPool_.add([f, &args..., this]()
        {
            try
            {
                f(std::forward<Args>(args)...);
            }
            catch (std::runtime_error error)
            {
                MessageBoxA(NULL, error.what(), "Runtime error", MB_OK);
            }
            catch (std::exception error)
            {
                MessageBoxA(NULL, error.what(), "Exception", MB_OK);
            }
            catch (...)
            {
                MessageBoxA(NULL, "Undefined exception", "Exception", MB_OK);
            }
        });
}

template<typename Func, typename ...Args>
auto ThreadPool::parallelize(std::size_t from, std::size_t to, std::size_t blockCount, Func&& f, Args && ...args)
{
    using functionType = decltype(f(from, to));
    MultiFuture<functionType> multi;

    const int increase = (to - from) / blockCount;
    for (std::size_t i = 0; i < to; i += increase)
    {
        int a = i;
        int b = a + increase;
    
        auto fut = submit(
            [f, a, b]() -> functionType
            {
                return [f](const int a, const int b)
                {
                    return f(a, b);
                }(a, b); 
            });

        multi.pushFuture(fut.share());
    }

    return multi;
}
