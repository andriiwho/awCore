#include "aw/core/async/thread_worker.h"

namespace aw::core
{

	ThreadWorker::ThreadWorker(usize num_threads)
		: m_Worker(&ThreadWorker::worker_loop, this)
	{
	}

	ThreadWorker::~ThreadWorker()
	{
		force_stop();

		if (m_Worker.joinable())
		{
			m_Worker.join();
		}
	}

	void ThreadWorker::wait() const
	{
		while (m_NumActiveTasks.load(std::memory_order::acquire) > 0)
		{
			std::this_thread::yield();
		}
	}

	void ThreadWorker::force_stop()
	{
		{
			std::lock_guard lock(m_TasksMutex);
			m_Stopping = true;
		}

		m_Semaphore.notify_one();
	}

	void ThreadWorker::worker_loop()
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
} // namespace aw::core