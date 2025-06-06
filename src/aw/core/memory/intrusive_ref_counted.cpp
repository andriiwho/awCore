#include "aw/core/memory/intrusive_ref_counted.h"

namespace aw::core
{
	void IntrusiveRefCounted::add_ref() const
	{
		m_RefCount.fetch_add(1, std::memory_order_relaxed);
	}
	void IntrusiveRefCounted::release() const
	{
		if (m_RefCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
		{
			aw_delete(const_cast<IntrusiveRefCounted*>(this));
		}
	}
	usize IntrusiveRefCounted::get_ref_count() const
	{
		return m_RefCount.load(std::memory_order_acquire);
	}

	void detail::fwd_ref_ptr_add_ref(void* ptr)
	{
		static_cast<IntrusiveRefCounted*>(ptr)->add_ref();
	}

	void detail::fwd_ref_ptr_release(void* ptr)
	{
		static_cast<IntrusiveRefCounted*>(ptr)->release();
	}
} // namespace aw::core