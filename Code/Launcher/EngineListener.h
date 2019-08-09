/**
 * @file
 * @brief Game engine listener.
 */

#pragma once

// CryEngine headers
#include "ISystem.h"

class EngineListener : public ISystemUserCallback
{
public:

	// --- ISystemUserCallback ---
	bool OnError( const char *szErrorString ) override;
	void OnSaveDocument() override;
	void OnProcessSwitch() override;
	void OnInitProgress( const char *sProgressMsg ) override;
	void OnInit( ISystem *pSystem ) override;
	void OnShutdown() override;
	void OnUpdate() override;
	void GetMemoryUsage( ICrySizer *pSizer ) override;
};

