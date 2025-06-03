#pragma once

#include "thread_pool.h"
#include "aw/core/primitive/macros.h"

#include <stdexcept>

namespace aw::core
{
	template<typename Fn, typename... Args>
	auto async(Fn&& fn, Args&&... args)
	{
		if (!g_global_thread_pool)
		{
			throw std::runtime_error("g_global_thread_pool is not initialized. Use aw::core::initialize_global_thread_pool() to initialize it.");
		}

		return g_global_thread_pool->submit_task(std::forward<Fn>(fn), std::forward<Args>(args)...);
	}

	template<typename Fn, typename... Args>
	auto async_wait(Fn&& fn, Args&&... args)
	{
		return async(std::forward<Fn>(fn), std::forward<Args>(args)...).get();
	}

	template<typename T>
	auto await(std::future<T>& future)
	{
		return future.get();
	}

	namespace helpers
	{
		template<typename Fn>
		struct AsyncTaskExecutor
		{
			AsyncTaskExecutor(Fn&& fn)
			{
				async(std::forward<Fn>(fn));
			}
		};

		template<typename Fn>
		struct AsyncAwaitTaskExecutor
		{
			AsyncAwaitTaskExecutor(Fn&& fn)
			{
				async_wait(std::forward<Fn>(fn));
			}
		};
	}
}

#define aw_async aw::core::helpers::AsyncTaskExecutor AW_CONCAT(aw_async_, __LINE__) =
#define aw_async_wait aw::core::helpers::AsyncAwaitTaskExecutor AW_CONCAT(aw_async_wait_, __LINE__) =