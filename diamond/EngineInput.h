#pragma once
class Window;
class IGameInput;
#include "IEngineInput.h"
#include "IEngineInputIntl.h"
#include <steam/steam_api.h>
#include <string>
#include <map>

class EngineInput : public IEngineInputIntl, public IEngineInput
{
	enum ConnectorConnectionAction {
		eCCA_NoController,
		eCCA_StillConnected,
		eCCA_ControllerConnected,
		eCCA_ControllerDisconnected,
		eCCA_ControllerChanged,
		eCCA_ControllerStillConnected
	};
public:
	EngineInput(Window* pWnd, IGameInput* pGameInput);
	virtual bool Initialise();
	virtual void DeinitialiseAndDelete();
	virtual void ProcessInput(float fDeltaTime);
	virtual void ShowBindingPanel();
	void ProcessMouseCallback(double fXPosIn, double fYPosIn);
	void ProcessScrollCallback(double fYOffset);
	void ProcessKeyCallback(int nKey, int nScancode, int nAction, int nMods);

	// IEngineInput interface overrides
public:
	virtual bool IsControllerActionActive(unsigned int unAction);
	virtual void GetControllerAnalogAction(unsigned int unAction, float* pfX, float* pfY);
	virtual bool RegisterDigitalAction(unsigned int unAction, const char* pszAction, bool bActionOnRelease);
	virtual bool RegisterAnalogAction(unsigned int unAction, const char* pszAction);
	virtual bool RegisterActionSet(unsigned int unActionSet, const char* pszActionSet);
	virtual void SetSteamControllerActionSet(unsigned int unActionSet);
	
	virtual bool KeyPressed(unsigned int unKeyCode);


private:
	bool ReadConfigFile();

	void PollSteamInput();
	ConnectorConnectionAction FindActiveSteamInputDevice();

	void CheckKey(int keyToCheck, const std::string& keyId);
	bool ActionIsOnRelease(unsigned int unAction);
	bool ActionIsCurrentlyPressed(unsigned int unAction);
	void SetActionIsCurrentlyPressed(unsigned int unAction, bool bPressed);

private:
	Window* m_pWnd;
	IGameInput* m_pGameInput;
	bool m_bInputConfigRead;
	unsigned int m_nPrevActiveControllerCount;
	
	InputHandle_t m_hActiveController;

	// An map of game 'enums' to steam handle for digital Steam Controller events
	std::map<unsigned int, InputDigitalActionHandle_t> m_mpControllerDigitalActionHandles;
	// A map of game 'enums' to steam handle for analog Steam Controller events
	std::map<unsigned int, InputAnalogActionHandle_t> m_mpControllerAnalogActionHandles;
	// A map of game 'enums' to steam handle for different Steam Controller action set configurations
	std::map<unsigned int, InputActionSetHandle_t> m_mpControllerActionSetHandles;

	std::map<unsigned int, bool> m_mpActionWhenReleased;
	std::map<unsigned int, bool> m_mpActionIsPressed;
};

