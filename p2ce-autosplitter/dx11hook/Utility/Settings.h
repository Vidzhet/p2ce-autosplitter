#pragma once

#include "dx11hook.h"

class Settings
{
public:
	bool bESPEnabled;

	bool bESPBox;
	bool bESPHealth; 
	bool bESPHealthText;
	bool bESPName;
	bool bESPDistance; 
	bool bESPHead;
	bool bESPEnemyOnly;
	bool bESPSnapline;
	bool bESPHasArmor;

	bool bVisualsCrosshair;
	bool bVisualsFPS;
	bool bVisualsDebugInfo;

	bool bAccuracyNoSpread;
	float fSpread;
	bool bAccuracyNoRecoil;
	float fRecoil;

	bool bAccuracyAimbot;
	bool bAccuracyAimShowFOV;
	float fAimFov;

	bool bAccuracyTrigger;

	bool Menu;

	static Settings* GetInstance();

private:
	Settings();
	~Settings();

	static Settings* m_pInstance;
};