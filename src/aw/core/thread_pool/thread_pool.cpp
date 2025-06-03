#include "aw/core/async/thread_pool.h"

#include "aw/core/memory/paged_memory_pool.h"
#include "aw/core/primitive/defer.h"

namespace aw::core
{
	ThreadPool* g_global_thread_pool = nullptr;

	ThreadPool::ThreadPool(const usize num_threads)
	{
		m_Workers.reserve(num_threads);
		for (usize index = 0; index < num_threads; ++index)
		{
			m_Workers.emplace_back(std::thread(&ThreadPool::worker_loop, this));
		}
	}

	ThreadPool::~ThreadPool()
	{
		force_stop();

		for (std::thread& worker : m_Workers)
		{
			if (worker.joinable())
			{
				worker.join();
			}
		}
	}

	void ThreadPool::wait_all() const
	{
		while (m_NumActiveTasks.load(std::memory_order::acquire) > 0)
		{
			std::this_thread::yield();
		}
	}

	void ThreadPool::force_stop()
	{
		{
			std::lock_guard lock(m_TasksMutex);
			m_Stopping = true;
		}

		m_Semaphore.notify_all();
	}

	void ThreadPool::worker_loop()
	{
		while (true)
		{
			std::function<void()> task;

			{
				std::unique_lock lock(m_TasksMutex);
				m_Semaphore.wait(lock, [&, this] { return m_Stopping || !m_Tasks.empty(); });

				if (m_Stopping && m_Tasks.empty())
					break;

				task = std::move(m_Tasks.front());
				m_Tasks.pop();
			}

			task();
			--m_NumActiveTasks;
		}
	}

	void initialize_global_thread_pool(const usize num_threads)
	{
		g_global_thread_pool = aw_new ThreadPool(num_threads);
	}

	void shutdown_global_thread_pool(const bool wait_for_tasks)
	{
		if (wait_for_tasks)
			g_global_thread_pool->wait_all();

		g_global_thread_pool->force_stop();
		aw_delete(g_global_thread_pool);

		g_global_thread_pool = nullptr;
	}
} // namespace aw::core