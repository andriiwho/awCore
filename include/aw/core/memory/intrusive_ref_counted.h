#pragma once

#include "paged_memory_pool.h"
#include "aw/core/primitive/numbers.h"

#include <atomic>

namespace aw::core
{
	template <typename T>
	class RefPtr;

	class IntrusiveRefCounted
	{
	public:
		virtual ~IntrusiveRefCounted() = default;

		void add_ref() const;
		void release() const;
		usize get_ref_count() const;

	private:
		mutable std::atomic<usize> m_RefCount{ 1 };
	};

	namespace detail
	{
		void fwd_ref_ptr_add_ref(void* ptr);
		void fwd_ref_ptr_release(void* ptr);
	}

	template <typename T>
	class RefPtr
	{
	public:
		RefPtr() noexcept
			: m_Ptr(nullptr) {}

		RefPtr(std::nullptr_t) noexcept
			: m_Ptr(nullptr) {}

		RefPtr(T* ptr) noexcept
			: m_Ptr(ptr) {}

		RefPtr(const RefPtr& other) noexcept
			: m_Ptr()
		{
			m_Ptr = other.m_Ptr;
			if (m_Ptr)
			{
				detail::fwd_ref_ptr_add_ref(m_Ptr);
			}
		}
		RefPtr(RefPtr&& other) noexcept
			: m_Ptr(other.m_Ptr) { other.m_Ptr = nullptr; }

		template <typename U>
		RefPtr(const RefPtr<U>& other) noexcept
			: m_Ptr(other.get())
		{
			if (m_Ptr)
				detail::fwd_ref_ptr_add_ref(m_Ptr);
		}

		~RefPtr()
		{
			if (m_Ptr)
				detail::fwd_ref_ptr_release(m_Ptr);
		}

		RefPtr& operator=(const RefPtr& other) noexcept
		{
			if (this != &other)
			{
				if (m_Ptr)
					detail::fwd_ref_ptr_release(m_Ptr);
				m_Ptr = other.m_Ptr;
				if (m_Ptr)
					detail::fwd_ref_ptr_add_ref(m_Ptr);
			}
			return *this;
		}

		RefPtr& operator=(RefPtr&& other) noexcept
		{
			if (this != &other)
			{
				if (m_Ptr)
					detail::fwd_ref_ptr_release(m_Ptr);
				m_Ptr = other.m_Ptr;
				other.m_Ptr = nullptr;
			}
			return *this;
		}

		template <typename U>
		RefPtr& operator=(const RefPtr<U>& other) noexcept
		{
			if (this != &other)
			{
				if (m_Ptr)
					detail::fwd_ref_ptr_release(m_Ptr);
				m_Ptr = other.get();
				if (m_Ptr)
					detail::fwd_ref_ptr_add_ref(m_Ptr);
			}
			return *this;
		}

		template <typename U>
		RefPtr& operator=(RefPtr<U>&& other) noexcept
		{
			if (this != &other)
			{
				if (m_Ptr)
					detail::fwd_ref_ptr_release(m_Ptr);
				m_Ptr = other.get();
				other.m_Ptr = nullptr;
			}

			return *this;
		}

		T* operator->() const noexcept { return m_Ptr; }
		T& operator*() const noexcept { return *m_Ptr; }
		T* get() const noexcept { return m_Ptr; }

		explicit operator bool() const noexcept { return m_Ptr != nullptr; }
		bool is_valid() const { return operator bool(); }
		void reset()
		{
			if (m_Ptr)
				detail::fwd_ref_ptr_release(m_Ptr);
			m_Ptr = nullptr;
		}

		T** release_and_get_address() noexcept
		{
			if (m_Ptr)
				detail::fwd_ref_ptr_release(m_Ptr);
			return &m_Ptr;
		}

		T** operator&() noexcept { return release_and_get_address(); }

	private:
		template <typename U>
		friend class RefPtr;

		T* m_Ptr{};
	};

	template<typename T, typename ... Args>
	T* new_ref_counted(Args&& ... args)
	{
		return aw_new T(std::forward<Args>(args)...);
	}

	template<typename T, typename ... Args>
	RefPtr<T> new_ref(Args&&... args)
	{
		return RefPtr<T>(new_ref_counted<T>(std::forward<Args>(args)...));
	}
} // namespace aw::core