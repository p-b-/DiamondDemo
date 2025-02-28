#pragma once
#include "IGameInput.h"

class Camera;

enum EDigitalInput
{
    eDigitalInput_Accelerate,
    eDigitalInput_Decelerate,
    eDigitalInput_Fire,
    eDigitalInput_PauseMenu
};

enum EAnalogInput {
    eAnalogInput_Camera,
    eAnalogInput_Move
};

enum EControllerActionSet
{
    eControllerActionSet_ShipControls,
    eControllerActionSet_MenuControls
};

class Window;
class IScene;

class GameInput :
    public IGameInput
{
public:
    GameInput(Window* pWnd, IScene* pScene);
    virtual ~GameInput();

public:
    virtual bool RegisterInputs(IEngineInput* pEngineInput);
    virtual void ProcessInput(IEngineInput* pEngineInput, float fDeltaTime);
    virtual void ProcessMouseMovement(float fXPos, float fYPos);
    virtual void ProcessMouseScroll(float fScroll);
    virtual void ProcessKeyEvent(int nKey, int nScancode, int nAction, int nMods);

    virtual void ControllerContected(int hActiveController);
    virtual void ControllerDisconnected();

protected:
    void ProcessAnalogCameraInput(float fX, float fY, float fZ, float fDeltaTime);
    void ProcessAnalogMovement(float fX, float fZ, float fDeltaTime);
        

protected:
    Window* m_pWnd;
    IScene* m_pScene;

    bool m_bControllerConnected;
    bool m_bFirstMouse;
    float m_fLastMouseX;
    float m_fLastMouseY;
};

