/**
 * @file
 * @brief Implementation of launcher task system.
 */

#include <new>
#include <deque>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// Launcher headers
#include "TaskSystem.h"
#include "ILauncherTask.h"

class LockGuard
{
	CRITICAL_SECTION *m_pCriticalSection;

public:
	LockGuard( CRITICAL_SECTION & criticalSection )
	: m_pCriticalSection(&criticalSection)
	{
		EnterCriticalSection( m_pCriticalSection );
	}

	~LockGuard()
	{
		LeaveCriticalSection( m_pCriticalSection );
	}
};

class TaskSystem::Impl
{
	std::deque<ILauncherTask*> m_queue;
	CRITICAL_SECTION m_criticalSection;

public:
	Impl()
	: m_queue(),
	  m_criticalSection()
	{
		InitializeCriticalSection( &m_criticalSection );
	}

	~Impl()
	{
		DeleteCriticalSection( &m_criticalSection );
	}

	void PushTask( ILauncherTask *pTask )
	{
		LockGuard lock( m_criticalSection );

		m_queue.push_back( pTask );
	}

	ILauncherTask *PopTask()
	{
		LockGuard lock( m_criticalSection );

		if ( m_queue.empty() )
		{
			return NULL;
		}
		else
		{
			ILauncherTask *pTask = m_queue.front();
			m_queue.pop_front();
			return pTask;
		}
	}
};

/**
 * @brief Constructor.
 */
TaskSystem::TaskSystem()
: m_impl(new Impl())
{
}

/**
 * @brief Destructor.
 */
TaskSystem::~TaskSystem()
{
	delete m_impl;
}

/**
 * @brief Adds task to the queue.
 * This function can be called from any thread.
 * @param pTask The task allocated on heap using the "new" operator.
 */
void TaskSystem::AddTask( ILauncherTask *pTask )
{
	if ( pTask )
	{
		m_impl->PushTask( pTask );
	}
}

/**
 * @brief Executes callbacks of all waiting tasks and removes them from the queue.
 * This function MUST be called only from main thread.
 */
void TaskSystem::ExecuteWaitingTasks()
{
	ILauncherTask *pTask = m_impl->PopTask();
	while ( pTask )
	{
		pTask->Run();
		// destroy the task
		delete pTask;

		pTask = m_impl->PopTask();
	}
}

