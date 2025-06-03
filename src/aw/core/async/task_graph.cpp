#include "aw/core/async/task_graph.h"

namespace aw::core
{
	TaskGraph::~TaskGraph()
	{
	}

	void TaskGraph::execute(ThreadPool* pool)
	{
		if (!pool)
		{
			throw std::runtime_error("Thread pool is null");
		}
		m_RunningTasks = m_Tasks.size();

		for (TaskNodePtr& task : m_Tasks)
		{
			if (task->remaining_dependencies == 0)
			{
				schedule_node(task.get(), pool);
			}
		}
	}

	void TaskGraph::wait_all()
	{
		if (m_RunningTasks == 0)
		{
			return;
		}

		std::unique_lock<std::mutex> lock(m_WaitMutex);
		m_Semaphore.wait(lock, [&]() { return m_RunningTasks == 0; });
	}

	bool TaskGraph::introduces_cycle(const TaskNode* a, TaskNode* b)
	{
		if (b == a)
		{
			return true;
		}

		for (const TaskNode* dep : a->dependents)
		{
			if (introduces_cycle(dep, b))
			{
				return true;
			}
		}

		return false;
	}

	void TaskGraph::schedule_node(TaskNode* node, ThreadPool* pool)
	{
		if (!node->task)
		{
			throw std::runtime_error("Task failed. No function was assigned.");
		}

		pool->submit_task([this, node, pool] {
			node->task();

			for (TaskNode* dependent : node->dependents)
			{
				if (--dependent->remaining_dependencies == 0)
				{
					schedule_node(dependent, pool);
				}
			}

			if (--m_RunningTasks == 0)
			{
				std::lock_guard lock(m_WaitMutex);
				m_Semaphore.notify_all();
			}
		});
	}
} // namespace aw::core