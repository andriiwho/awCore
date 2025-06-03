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
		explicit ThreadWorker(usize num_threads = std::thread::hardware_concurrency());
		~ThreadWorker();

		template <typename Fn, typename... Args>
		auto submit_task(Fn&& fn, Args&&... args)
			-> std::future<std::invoke_result_t<Fn, Args...>>
		{
			using ReturnType = std::invoke_result_t<Fn, Args...>;

			DefaultAllocator<std::packaged_task<ReturnType()>> allocator;
			auto task = std::allocate_shared<std::packaged_task<ReturnType()>>(
				allocator,
				[func = std::move(fn), ... captured_args = std::forward<Args>(args)]() {
					return func(std::forward<Args>(captured_args)...);
				});

			std::future<ReturnType> result = task->get_future();
			{
				std::lock_guard lock(m_TasksMutex);
				m_Tasks.emplace([task]() { (*task)(); });
				++m_NumActiveTasks;
			}

			m_Semaphore.notify_one();
			return result;
		}

		void wait() const;
		void force_stop();

	private:
		void worker_loop();

		std::thread m_Worker{};
		Queue<std::function<void()>> m_Tasks{};
		std::mutex m_TasksMutex{};
		std::condition_variable m_Semaphore{};
		bool m_Stopping{};

		std::atomic<usize> m_NumActiveTasks{};
	};
}