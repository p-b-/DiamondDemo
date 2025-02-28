#pragma once
class IEngineInput
{
public:
	virtual bool IsControllerActionActive(unsigned int unAction) = 0;
	virtual void GetControllerAnalogAction(unsigned int unAction, float* pfX, float* pfY) = 0;
	virtual bool RegisterDigitalAction(unsigned int unAction, const char* pszAction, bool bActionOnRelease) = 0;
	virtual bool RegisterAnalogAction(unsigned int unAction, const char* pszAction) = 0;
	virtual bool RegisterActionSet(unsigned int unActionSet, const char* pszActionSet) = 0;
	virtual void SetSteamControllerActionSet(unsigned int unActionSet) = 0;

	virtual bool KeyPressed(unsigned int unKeyCode) = 0;
};

