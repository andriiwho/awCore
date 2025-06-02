#include "aw/core/async/thread_worker.h"

namespace aw::core
{

	ThreadWorker::ThreadWorker()
		: m_Thread(&ThreadWorker::thread_main, this)
	{
	}

	void ThreadWorker::thread_main()
	{
		m_ThreadID = std::this_thread::get_id();
		while (m_Running.load(std::memory_order::acquire))
		{
			if (!m_Running.load(std::memory_order::relaxed))
			{
				break;
			}
		}
	}
} // namespace aw::core