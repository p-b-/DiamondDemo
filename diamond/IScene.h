#pragma once

class Window;
class Textures;
class IGameLoop;

class IScene {
public:
	virtual bool Initialise(Textures* pTextureCtrl, Window* pWnd) = 0;
	virtual bool GetUsingShadows() = 0;
	virtual void SetGameLoop(IGameLoop* pGameLoop) = 0;
	virtual void Deinitialise() = 0;
	virtual void AnimateScene(float fDeltaTime) = 0;
	virtual void DrawScene(bool bForShadows) = 0;

	virtual void ControllerConnected() = 0;
	virtual void ProcessDigitalAction(unsigned int unAction, float fDeltaTime) = 0;
	virtual void ProcessAnalogAction(unsigned int unAction, float fAnalogValue, float fDeltaTime) = 0;
	virtual void ProcessAnalogAction(unsigned int unAction, float fAnalogValue1, float fAnalogValue2, float fDeltaTime) = 0;
	virtual void ProcessAnalogAction(unsigned int unAction, float fAnalogValue1, float fAnalogValue2, float fAnalogValue3, float fDeltaTime) = 0;
};