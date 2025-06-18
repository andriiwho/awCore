#pragma once

#include <functional>
#include <type_traits>
#include <algorithm>

namespace aw::core
{
	template <typename... Args>
	class Delegate
	{
	public:
		using FuncType = std::function<void(Args...)>;

		template <typename Fn>
			requires std::is_invocable_v<Fn, Args...>
		void bind(Fn&& in_func)
		{
			m_Func = std::forward<Fn>(in_func);
		}

		template <typename Handler>
		void bind(Handler* handler, void (Handler::*func)(Args...))
		{
			m_Func = [handler, func](Args... args) { (handler->*func)(args...); };
		}

		void execute_safe(Args... args)
		{
			if (m_Func)
				m_Func(args...);
		}

		void execute(Args... args)
		{
			m_Func(args...);
		}

		void operator()(Args... args)
		{
			execute_safe(args...);
		}

		void clear()
		{
			m_Func = {};
		}

	private:
		FuncType m_Func{};
	};

	using DelegateVoid = Delegate<>;

	enum class DelegateHandle : core::u64
	{
	};

	template <typename... Args>
	class MulticastDelegate
	{
	public:
		using FuncType = std::function<void(Args...)>;

	private:
		struct FunctionWithHandler
		{
			FuncType func{};
			void* handler{};
		};

	public:
		template <typename Fn>
			requires std::is_invocable_v<Fn, Args...>
		DelegateHandle add(Fn&& in_func)
		{
			FunctionWithHandler handler {
				.func = std::forward<Fn>(in_func),
				.handler = nullptr
			};

			const auto handle = DelegateHandle{m_Functions.size()};
			m_Functions.push_back(std::move(handler));
			return handle;
		}

		template<typename Handler>
		DelegateHandle add(Handler* handler_object, void (Handler::*func)(Args...))
		{
			FunctionWithHandler handler {
				.func = [handler_object, func](Args... args) { (handler_object->*func)(args...); },
				.handler = handler_object
			};

			const auto handle = DelegateHandle{m_Functions.size()};
			m_Functions.push_back(std::move(handler));
			return handle;
		}

		void remove(void* handler_object)
		{
			if (m_Functions.empty())
			{
				return;
			}

			if (const auto iter = std::find_if(m_Functions.begin(), m_Functions.end(), [handler_object](const auto& func) { return func.handler == handler_object; });
				iter != m_Functions.end())
			{
				(*iter).handler = nullptr;
				(*iter).func = {};
			}
		}

		void remove(DelegateHandle handle)
		{
			if (m_Functions.empty())
			{
				return;
			}

			const usize index = static_cast<usize>(handle);
			if (m_Functions.size() > index)
			{
				m_Functions[index] = {};
			}
		}

		void execute_safe(Args... args)
		{
			for (const auto& func : m_Functions)
			{
				if (func.func)
					func.func(args...);
			}
		}

		void execute(Args... args)
		{
			for (const auto& func : m_Functions)
			{
				func.func(args...);
			}
		}

		void operator()(Args... args)
		{
			execute_safe(args...);
		}

	private:
		std::vector<FunctionWithHandler> m_Functions;
	};
} // namespace aw::core