#pragma once
#include "dx11hook.h"

class Input
{
public:
	static Input* GetInstance();

	void StartThread();
	void StopThread();

private:
	Input();
	~Input();

    static void MWheelMonitor();
	static void MenuKeyMonitor();

	HANDLE m_hThread;
	HANDLE mw_hThread;

	static Input* m_pInstance;
};
