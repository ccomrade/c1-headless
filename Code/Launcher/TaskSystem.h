/**
 * @file
 * @brief Launcher task system.
 */

#pragma once

struct ILauncherTask;

class TaskSystem
{
	class Impl;
	Impl *m_impl;  // std::unique_ptr is C++11

public:
	TaskSystem();
	~TaskSystem();

	void AddTask( ILauncherTask *pTask );

	void ExecuteWaitingTasks();
};

