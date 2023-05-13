#include "gtest/gtest.h"
#include "ThreadPool.h"
#include <Windows.h>

TEST(ThreadPoolTest, SubmitFuncReturn)
{
	ThreadPool pool;
	auto future = pool.submit([]()
		{
			return 25;
		});

	auto res = future.get();
	EXPECT_EQ(25, res);
}

TEST(ThreadPoolTest, PushFuncReturn)
{
	ThreadPool pool;
	std::vector<int> vector;
	pool.pushTask([](std::vector<int>& vectorTest)
		{
			vectorTest.push_back(5);
		}, std::ref(vector));

	pool.waitForAllTasks();

	EXPECT_EQ(5, vector.back());
}

TEST(ThreadPoolTest, Adding5TasksWith5Threads)
{
	constexpr std::size_t tasksCount = 5;
	constexpr std::size_t threadsCount = 5;
	ThreadPool pool(threadsCount);
	std::vector<int> vector;
	vector.resize(tasksCount);

	for (std::size_t i = 0; i < tasksCount; ++i)
	{
		pool.pushTask([i](std::vector<int>& vectorTest)
			{
				vectorTest[i] = 1;
			}, std::ref(vector));
	}
	pool.waitForAllTasks();

	for (auto& el : vector)
		EXPECT_EQ(1, el);
	EXPECT_EQ(threadsCount, pool.threadsCount());
}

TEST(ThreadPoolTest, ParallelizeFunc)
{
	ThreadPool pool(2);
	std::vector<int> vector;
	vector.resize(100);

	pool.parallelize(0, 100, 2, [&vector](const int a, const int b)
		{
			for (int i = a; i < b; ++i)
				vector[i] = 1;
		});
	pool.waitForAllTasks();

	for (auto& el : vector)
		EXPECT_EQ(1, el);
}

TEST(ThreadPoolTest, ParallelizeFuncReturn)
{
	ThreadPool pool(2);

	auto t = pool.parallelize(0, 100, 2, [](const int a, const int b) -> int
		{
			int total = 0;
			for (int i = a; i < b; ++i)
				total += 1;
			return total;
		});

	int total = 0;
	for (auto& el : t.get())
		total += el;

	EXPECT_EQ(100, total);
}

TEST(Constructing, SuccessfullConstructingWith5ThreadIDs)
{
	constexpr std::size_t ThreadsCount = 5;
	ThreadPool ThreadPool_(ThreadsCount);

	EXPECT_EQ(ThreadPool_.threadsCount(), ThreadsCount);
}

TEST(ThreadPoolTest, UndefExceptionTest)
{
	ThreadPool pool;

	pool.run();

	EXPECT_EQ(1, pool.getMessages(MessagePool::Message::Type::Info).size());
}

