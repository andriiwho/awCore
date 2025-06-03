#include "aw/core/async/async.h"

#include <aw/core/all.h>
#include <gtest/gtest.h>

using namespace aw::core;

TEST(ThreadPoolTests, TestSubmitTask)
{
	ThreadPool pool{};

	Vector<usize> v;
	std::mutex v_mutex;

	for (i32 index = 0; index < 1000; ++index)
	{
		pool.submit_task([index, &v, &v_mutex] {
			std::lock_guard lock(v_mutex);
			v.emplace_back(index);
		});
	}

	pool.wait_all();
	EXPECT_EQ(v.size(), 1000);
	for (const i32 index : v)
	{
		EXPECT_TRUE(std::ranges::find(v, index) != v.end());
	}
}

TEST(ThreadPoolTests, TestGlobalThreadPool)
{
	aw_init_global_thread_pool_scoped();

	Vector<usize> v;
	std::mutex v_mutex;

	for (i32 index = 0; index < 1000; ++index)
	{
		g_global_thread_pool->submit_task([index, &v, &v_mutex] {
			std::lock_guard lock(v_mutex);
			v.emplace_back(index);
		});
	}

	g_global_thread_pool->wait_all();
	EXPECT_EQ(v.size(), 1000);
	for (const i32 index : v)
	{
		EXPECT_TRUE(std::ranges::find(v, index) != v.end());
	}
}

TEST(AsyncTests, TestAsyncWait)
{
	aw_init_global_thread_pool_scoped();

	try
	{
		const auto result = async_wait([] {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			return 35;
		});
		EXPECT_EQ(result, 35);
	}
	catch (const std::exception& e)
	{
		FAIL() << e.what() << std::endl;
	}

	try
	{
		auto result2 = async([] {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			return 35;
		});
		EXPECT_EQ(await(result2), 35);
	}
	catch (const std::exception& e)
	{
		FAIL() << e.what() << std::endl;
	}
}

TEST(AsyncTests, TestAsyncMacros)
{
	aw_init_global_thread_pool_scoped();

	try
	{
		int value = 0;
		aw_async[&value]
		{
			value = 55;
		};

		int value2 = 0;
		aw_async_wait([&value2] { value2 = 55; });
		EXPECT_EQ(value2, 55);

		g_global_thread_pool->wait_all();
		EXPECT_EQ(value, 55);
		EXPECT_EQ(value2, 55);
	}
	catch (const std::exception& e)
	{
		FAIL() << e.what() << std::endl;
	}
}

TEST(ThreadWorkerTests, TestThreadWorker)
{
	ThreadWorker worker{};

	int value = 0;
	int value2 = 0;
	worker.submit_task([&value] { value = 69; });
	worker.submit_task([&value2] { value2 = 69; });

	worker.wait();

	EXPECT_EQ(value, 69);
	EXPECT_EQ(value2, 69);
}

TEST(TaskGraphTests, TestTaskGraph)
{
	aw_init_global_thread_pool_scoped();

	TaskGraph graph;

	Vector<usize> v;
	std::mutex v_mutex;

	auto* t1 = graph.add_task([&] { v.push_back(0); });
	auto* t2 = graph.add_task([&] { v.push_back(1); });
	auto* t3 = graph.add_task([&] { v.push_back(2); });

	t1->then(t2)->then(t3);

	graph.execute(g_global_thread_pool);
	graph.wait_all();

	EXPECT_EQ(v.size(), 3);

	usize i = 0;
	for (const usize x : v)
	{
		EXPECT_EQ(x, i++);
	}
}

TEST(TaskGraphTests, TestTaskGraphThen)
{
	aw_init_global_thread_pool_scoped();

	TaskGraph graph;

	Vector<usize> v;
	std::mutex v_mutex;

	const auto last_task = graph.add_task([&] { v.push_back(0); })
		->then([&] { v.push_back(1); })
		->then([&] { v.push_back(2); });

	const auto new_task = graph.add_task([&] { v.push_back(3); });
	last_task->then(new_task);

	graph.execute(g_global_thread_pool);
	graph.wait_all();

	EXPECT_EQ(v.size(), 4);

	usize i = 0;
	for (const usize x : v)
	{
		EXPECT_EQ(x, i++);
	}
}