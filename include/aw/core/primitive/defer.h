#pragma once
#include <utility>

namespace aw::core
{
	template <typename F>
	class defer_scope_guard
	{
		F	 f;
		bool active = true;

	public:
		defer_scope_guard(F&& f) noexcept
			: f(std::forward<F>(f))
		{
		}

		defer_scope_guard(defer_scope_guard&& other) noexcept
			: f(std::move(other.f)), active(other.active)
		{
			other.active = false;
		}

		~defer_scope_guard() noexcept
		{
			if (active)
				f();
		}

		defer_scope_guard(const defer_scope_guard&) = delete;

		defer_scope_guard& operator=(const defer_scope_guard&) = delete;
	};

	template <typename F>
	[[nodiscard]] auto scope_exit(F&& f)
	{
		return defer_scope_guard<F>(std::forward<F>(f));
	}
} // namespace aw::core

#define AW_DEFER_CONCAT_IMPL(x, y) x##y
#define AW_DEFER_CONCAT(x, y) AW_DEFER_CONCAT_IMPL(x, y)
#define defer aw::core::defer_scope_guard AW_DEFER_CONCAT(defer_, __LINE__) =
