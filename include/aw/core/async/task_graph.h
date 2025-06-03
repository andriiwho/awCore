#pragma once

#include "thread_pool.h"
#include "aw/core/memory/paged_memory_pool.h"
#include "aw/core/primitive/container_aliases.h"

namespace aw::core
{
	class TaskGraph
	{
	public:
		struct TaskNode;
		static bool introduces_cycle(const TaskNode* a, TaskNode* b);

		struct TaskNode
		{
			TaskGraph* parent{};

			std::function<void()> task{};
			Vector<TaskNode*> dependents{};
			std::atomic<usize> remaining_dependencies{};

			template<typename Fn>
			TaskNode* then(Fn&& fn)
			{
				if (!parent)
				{
					throw std::runtime_error("Parent is null. This is probably a bug.");
				}

				TaskNode* dependent = parent->add_task(std::move(fn));
				if (introduces_cycle(dependent, this))
				{
					throw std::logic_error("Task graph task introduces cycle.");
				}

				dependents.push_back(dependent);
				++dependent->remaining_dependencies;

				return dependent;
			}

			TaskNode* then(TaskNode* dependent)
			{
				if (introduces_cycle(dependent, this))
				{
					throw std::logic_error("Task graph task introduces cycle.");
				}

				dependents.push_back(dependent);
				++dependent->remaining_dependencies;
				return dependent;
			}
		};

	private:
		struct TaskNodeDeleter
		{
			void operator()(TaskNode* node) const
			{
				aw_delete(node);
			}
		};

	public:

		using TaskNodePtr = std::unique_ptr<TaskNode, TaskNodeDeleter>;

		~TaskGraph();

		template<typename Fn>
		TaskNode* add_task(Fn&& fn)
		{
			auto task = TaskNodePtr(aw_new TaskNode);
			task->task = [func = std::move(fn)] { func(); };
			const auto task_ptr = task.get();
			task->parent = this;
			return m_Tasks.emplace_back(std::move(task)).get();
		}

		void execute(ThreadPool* pool);
		void wait_all();

	private:
		void schedule_node(TaskNode* node, ThreadPool* pool);

		Vector<TaskNodePtr> m_Tasks{};
		std::atomic<usize> m_RunningTasks{};
		std::mutex m_WaitMutex{};
		std::condition_variable m_Semaphore{};
	};
}