#pragma once

#include "aw/core/primitive/container_aliases.h"
#include "aw/core/primitive/numbers.h"

#include <thread>
#include <atomic>
#include <functional>
#include <mutex>
#include <future>

namespace aw::core
{
	class ThreadWorker
	{
	public:
		ThreadWorker();

		template<typename F>
		std::future<void> submit_task(F&& f)
		{
			return {};
		}

	private:
		void thread_main();

		std::thread m_Thread;
		std::thread::id m_ThreadID{};
		std::atomic_bool m_Running = true;

		Queue<std::function<void()>> m_Tasks;
		std::mutex m_TasksMutex{};
		std::condition_variable m_TasksCV{};
	};
}