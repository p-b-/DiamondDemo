#pragma once
class IEngineInput;
class Camera;
class IGameInput
{
public:
	virtual bool RegisterInputs(IEngineInput* pInput) = 0;
	virtual void ProcessInput(IEngineInput* pInput, float fDeltaTime) = 0;
	virtual void ProcessMouseMovement(float fXPos, float fYPos) = 0;
	virtual void ProcessMouseScroll(float fScroll) = 0;
	virtual void ProcessKeyEvent(int nKey, int nScancode, int nAction, int nMods) = 0;
	virtual void ControllerContected(int hActiveController) = 0;
	virtual void ControllerDisconnected() = 0;
};

