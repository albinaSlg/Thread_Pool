#include "gtest/gtest.h"
#include "ThreadPool.h"
#include <Windows.h>

TEST(ThreadPoolTest, SubmitFuncReturn)
{
	ThreadPool pool(1);
	auto future = pool.submit([]()
		{
			return 25;
		});

	auto res = future.get();
	EXPECT_EQ(res, 25);
}

TEST(ThreadPoolTest, PushFuncReturn)
{
	ThreadPool pool(2);
	std::vector<int> vector;
	pool.pushTask([](std::vector<int>& vectorTest)
		{
			vectorTest.push_back(5);
		}, vector);

	//Sleep(100);
	pool.stop();
	std::cout << vector.back() << std::endl;

	EXPECT_EQ(vector.back(), 5);
}

