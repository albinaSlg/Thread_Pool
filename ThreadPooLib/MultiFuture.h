#pragma once
#include <future>
#include <vector>
#include <shared_mutex>

template<typename T>
class MultiFuture
{
public:
	 MultiFuture() = default;
	 MultiFuture(const MultiFuture &multi);
	 MultiFuture(MultiFuture &&multi);

	void pushFuture(std::shared_future<T> future);
	_NODISCARD std::vector <T> get();
	void wait() const;
private:
    mutable std::shared_mutex mutex_;
	std::vector <std::shared_future<T>> futures_;
};

template <typename T>
inline MultiFuture<T>::MultiFuture(const MultiFuture &multi)
{
        futures_ = multi.futures_;
}

template <typename T> inline MultiFuture<T>::MultiFuture(MultiFuture &&multi) 
{
        futures_ = std::move(multi.futures_);
}

template <typename T>
inline void MultiFuture<T>::pushFuture(std::shared_future<T> future)
{
    std::lock_guard lock(mutex_);

	futures_.push_back(future);
}

template<typename T>
std::vector<T> MultiFuture<T>::get()
{
	std::vector<T> vector;
    std::shared_lock lock(mutex_);

	for (auto& future : futures_)
		vector.emplace_back(future.get());
	return vector;
}

template<typename T>
void MultiFuture<T>::wait() const
{
    std::shared_lock lock(mutex_);

	for (auto& future : futures_)
		future.wait();
}

