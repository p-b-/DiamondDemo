#include "GameInput.h"
#include "IScene.h"
#include "IEngineInput.h"
#include "Camera.h"
#include "Window.h"
#include "SceneActions.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdlib>

GameInput::GameInput(Window* pWnd, IScene* pScene) {
    m_pWnd = pWnd;
    m_pScene = pScene;
    m_bFirstMouse = true;
    m_fLastMouseX = 0.0f;
    m_fLastMouseY = 0.0f;
}

GameInput::~GameInput() {

}

bool GameInput::RegisterInputs(IEngineInput* pEngineInput) {
    if (!pEngineInput->RegisterDigitalAction(eDigitalInput_Accelerate, "accelerate", false)) {
        return false;
    }
    if (!pEngineInput->RegisterDigitalAction(eDigitalInput_Decelerate, "decelerate", false)) {
        return false;
    }
    if (!pEngineInput->RegisterDigitalAction(eDigitalInput_Fire, "mainfire", true)) { 
        return false;  
    }
    if (!pEngineInput->RegisterDigitalAction(eDigitalInput_PauseMenu, "pause_menu", true)) { 
        return false;
    }

    if (!pEngineInput->RegisterAnalogAction(eAnalogInput_Camera, "camera")) { 
        return false;
    }
    if (!pEngineInput->RegisterAnalogAction(eAnalogInput_Move, "move")) { 
        return false;
    }

    if (!pEngineInput->RegisterActionSet(eControllerActionSet_ShipControls, "InGameControls")) { 
        return false;
    }
    if (!pEngineInput->RegisterActionSet(eControllerActionSet_MenuControls, "MenuControls")) { 
        return false;
    }
    return true;
}

void GameInput::ControllerContected(int hActiveController) {
    m_bControllerConnected = true;
    m_pScene->ControllerConnected();
}

void GameInput::ControllerDisconnected() {
    m_bControllerConnected = false;
}

void GameInput::ProcessInput(IEngineInput* pEngineInput, float fDeltaTime) {
    pEngineInput->SetSteamControllerActionSet(eControllerActionSet_ShipControls);
    float cameraSpeed = static_cast<float>(2.5 * fDeltaTime);
    float deltaTime = m_pWnd->GetDeltaTime();

    if (pEngineInput->KeyPressed(GLFW_KEY_ESCAPE)) {
        m_pScene->ProcessDigitalAction(eSceneAction_Quit, deltaTime);
        return;
        m_pWnd->ShouldClose(true);
    }

    if (m_bControllerConnected) {
        if (pEngineInput->IsControllerActionActive(eDigitalInput_PauseMenu)) {
            m_pScene->ProcessDigitalAction(eSceneAction_ShowDepthMap, deltaTime);
        }
        if (pEngineInput->IsControllerActionActive(eDigitalInput_Accelerate)) {
            m_pScene->ProcessDigitalAction(eSceneAction_ShowBindingPanel, deltaTime);
        }
        if (pEngineInput->IsControllerActionActive(eDigitalInput_Fire)) {
            m_pScene->ProcessDigitalAction(eSceneAction_Fire, deltaTime);
        }

        if (pEngineInput->IsControllerActionActive(eDigitalInput_Accelerate)) {
            std::cout << "Accelerate" << std::endl;
        }
        if (pEngineInput->IsControllerActionActive(eDigitalInput_Decelerate)) {
            std::cout << "Decelerate" << std::endl;
        }
        if (pEngineInput->IsControllerActionActive(eDigitalInput_Fire)) {
        }
    }

    bool keyMovement = false;
    bool upDownMovement = false;
    float dx = 0.0f;
    float dz = 0.0f;
    float dy = 0.0f;
    float speed = 1.0;
    if (m_pWnd->GetKey(GLFW_KEY_LEFT_SHIFT)) {
        speed = speed / 5.0f;
    }
    if (m_pWnd->GetKey(GLFW_KEY_W) == GLFW_PRESS) {
        dz += speed;
        keyMovement = true;
    }
    if (pEngineInput->KeyPressed(GLFW_KEY_S)) {
        dz -= speed;
        keyMovement = true;
    }
    if (pEngineInput->KeyPressed(GLFW_KEY_A)) {
        dx -= speed;
        keyMovement = true;
    }
    if (pEngineInput->KeyPressed(GLFW_KEY_D)) {
        dx += speed;
        keyMovement = true;
    }
    if (pEngineInput->KeyPressed(GLFW_KEY_F)) {
        dy += speed;
        upDownMovement = true;
    }
    if (pEngineInput->KeyPressed(GLFW_KEY_C)) {
        dy -= speed;
        upDownMovement = true;
    }

    float x;
    float y;
    if (keyMovement) {
        ProcessAnalogMovement(dx, dz, fDeltaTime);
    }
    else if (m_bControllerConnected) {
        pEngineInput->GetControllerAnalogAction(eAnalogInput_Move, &x, &y);
        if (std::abs(x) > 0.01f || std::abs(y) > 0.01f) {
            ProcessAnalogMovement(x, y, fDeltaTime);
        }
    }
    if (m_bControllerConnected) {
        pEngineInput->GetControllerAnalogAction(eAnalogInput_Camera, &x, &y);
    }
    else {
        x = 0.0f;
        y = 0.0f;
    }
    if (std::abs(x) > 0.01f || std::abs(y) > 0.01f || upDownMovement) {
        ProcessAnalogCameraInput(x, dy, y, fDeltaTime);
    }
}

void GameInput::ProcessMouseMovement(float fXPos, float fYPos) {
    if (m_bFirstMouse)
    {
        m_fLastMouseX = fXPos;
        m_fLastMouseY = fYPos;
        m_bFirstMouse = false;
    }

    float xoffset = fXPos - m_fLastMouseX;
    float yoffset = m_fLastMouseY - fYPos;
    m_fLastMouseX = fXPos;
    m_fLastMouseY = fYPos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;
}

void GameInput::ProcessKeyEvent(int nKey, int nScancode, int nAction, int nMods) {
}


void GameInput::ProcessMouseScroll(float fScroll) {
    m_pScene->ProcessAnalogAction(eSceneAction_Zoom, -fScroll, 0.0f);
}

void GameInput::ProcessAnalogCameraInput(float fX, float fY,float fZ, float fDeltaTime) {
    m_pScene->ProcessAnalogAction(eSceneAction_MoveCamera, fX / 10.0f, fY, fZ/10.0f, fDeltaTime);
}

void GameInput::ProcessAnalogMovement(float fX, float fZ, float fDeltaTime) {
    m_pScene->ProcessAnalogAction(eSceneAction_Move, fX * 4.0f, fZ * 4.0f , fDeltaTime);
}