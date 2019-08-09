/**
 * @file
 * @brief Launcher task interface.
 */

#pragma once

struct ILauncherTask
{
	virtual ~ILauncherTask()
	{
	}

	virtual void Run() = 0;
};

