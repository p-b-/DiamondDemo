// /home/deck/.local/share/Steam/steamapps/common/Steam Controller Configs/37422590/config/480/controller_neptune.vdf


#include "EngineInput.h"
#include "Window.h"
#include "IGameInput.h"
#include <direct.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#ifdef _DEBUG
int gnActionSetError = 0;
#endif

void key_callback(GLFWwindow* pGLWnd, int nKey, int nScancode, int nAction, int nMods);
void mouse_callback(GLFWwindow* pGLWnd, double xpos, double ypos);
void scroll_callback(GLFWwindow* pGLWnd, double xoffset, double yoffset);
void focus_callback(GLFWwindow* pGLWnd, int focused);

bool LogEvery(int& count, int every) {
    if (count == -1) {
        count = 0;
        return true;
    }
    ++count;
    if (++count == every) {
        count = 0;
        return true;
    }
    return false;
}

int gLogSetActionSet = -1;
int gLogGetControllerAnalogAction = -1;

EngineInput::EngineInput(Window* pWnd, IGameInput* pGameInput) {
    m_pWnd = pWnd;
    m_pGameInput = pGameInput;
    m_hActiveController = 0;
    m_nPrevActiveControllerCount = 0;
    m_bInputConfigRead = false;

    m_pWnd->SetMouseCallback(mouse_callback);
    m_pWnd->SetScrollCallback(scroll_callback);
    m_pWnd->SetKeyCallback(key_callback);
    m_pWnd->SetFocusCallback(focus_callback);
//    m_pWnd->CaptureMouse();
    m_pWnd->SetInput(this);
}

bool EngineInput::Initialise() {
    char rgchCWD[1024];
    if (!_getcwd(rgchCWD, sizeof(rgchCWD)))
    {
        strcpy(rgchCWD, ".");
    }

    char rgchFullPath[1024];
#if defined(OSX)
    // hack for now, because we do not have utility functions available for finding the resource path
    // alternatively we could disable the SteamController init on OS X
    _snprintf(rgchFullPath, sizeof(rgchFullPath), "%s/steamworksexample.app/Contents/Resources/%s", rgchCWD, "steam_input_manifest.vdf");
#else
    _snprintf(rgchFullPath, sizeof(rgchFullPath), "%s\\%s", rgchCWD, "steam_input_manifest.vdf");
#endif
    std::cout << "Manifest: " << rgchFullPath << std::endl;
    bool setCorrectly = SteamInput()->SetInputActionManifestFilePath(rgchFullPath);
    std::cout << "Set input action manifest: "<<setCorrectly<<std::endl;

    //    m_hInGame = SteamInput()->GetActionSetHandle("InGameControls");

    // clear the action handles
    m_mpControllerDigitalActionHandles.clear();
    m_mpControllerActionSetHandles.clear();
    m_mpControllerAnalogActionHandles.clear();
    // To be used when support origins:
//    m_ControllerDigitalActionOrigins.clear();
//    m_ControllerAnalogActionOrigins.clear();
    m_hActiveController = 0;
    m_bInputConfigRead = ReadConfigFile();
    if (m_bInputConfigRead) {
        // Used to signal to user that action has happened.
    //    m_pGameInput->ControllerContected(0);
    }

    if (!SteamInput()->Init(false))
    {
        return false;
    }
    return true;
}

void EngineInput::DeinitialiseAndDelete() {
    SteamInput()->Shutdown();
    delete this;
}

bool EngineInput::ReadConfigFile() {
    return m_pGameInput->RegisterInputs(this);
}

void EngineInput::PollSteamInput() {
    // There's a bug where the action handles aren't non-zero until a config is done loading. Soon config
    // information will be available immediately. Until then try to init as long as the handles are invalid.

    if (!m_bInputConfigRead && m_mpControllerDigitalActionHandles.size() == 0) {
        if (ReadConfigFile()) {
            std::cout << "EngineInput::PollSteamInput() Read config file" << std::endl;
            m_bInputConfigRead = true;
        }
    }

    auto connectionAction = FindActiveSteamInputDevice();

    switch (connectionAction) {
    case eCCA_ControllerConnected:
        if (!m_bInputConfigRead && ReadConfigFile()) {
            std::cout << "EngineInput::PollSteamInput() Read config file after finding controller" << std::endl;
            m_bInputConfigRead = true;
        }
        m_pGameInput->ControllerContected(m_hActiveController);
        break;
    case eCCA_ControllerDisconnected:
        m_hActiveController = 0;
        m_bInputConfigRead = false;
        m_pGameInput->ControllerDisconnected();
        break;
    case eCCA_ControllerChanged:
        m_bInputConfigRead = ReadConfigFile();
        m_pGameInput->ControllerContected(m_hActiveController);
        break;
    }
}

void EngineInput::ProcessInput(float fDeltaTime) {
    PollSteamInput();
    m_pGameInput->ProcessInput(this, fDeltaTime);
}

void EngineInput::CheckKey(int keyToCheck, const std::string& keyId) {
    int v = m_pWnd->GetKey(keyToCheck);
    if (v != 0) {
        std::cout << keyId.c_str() << " button " << v << std::endl;
    }
}

void EngineInput::ShowBindingPanel() {
    SteamInput()->ShowBindingPanel(m_hActiveController);
}

EngineInput::ConnectorConnectionAction EngineInput::FindActiveSteamInputDevice() {
    InputHandle_t prevController = m_hActiveController;

    // Use the first available steam controller for all interaction. We can call this each frame to handle
    // a controller disconnecting and a different one reconnecting. Handles are guaranteed to be unique for
    // a given controller, even across power cycles.

    // See how many Steam Controllers are active. 
    InputHandle_t pHandles[STEAM_CONTROLLER_MAX_COUNT];
    int nNumActive = SteamInput()->GetConnectedControllers(pHandles);
    if (nNumActive == 0 && m_nPrevActiveControllerCount > 0) {
        std::cout << "Controller disconnected" << std::endl;
        m_hActiveController = 0;
        m_nPrevActiveControllerCount = 0;
        return eCCA_ControllerDisconnected;
    }


    if (nNumActive != m_nPrevActiveControllerCount) {
        std::cout << "Active input device count: " << nNumActive << std::endl;
        m_nPrevActiveControllerCount = nNumActive;
    }

    // If there's an active controller, and if we're not already using it, select the first one.
    if (nNumActive && (m_hActiveController != pHandles[0])) {
        m_hActiveController = pHandles[0];

        if (prevController == 0) {
            std::cout << "Found controller" << std::endl;
            return eCCA_ControllerConnected;
        }
        else {
            std::cout << "Controller changed" << std::endl;
            return eCCA_ControllerChanged;
        }
    }
    if (nNumActive > 0) {
        return eCCA_ControllerStillConnected;
    }
    else {
        return eCCA_NoController;
    }
}

bool EngineInput::RegisterDigitalAction(unsigned int unAction, const char* pszAction, bool bActionOnRelease) {
    InputDigitalActionHandle_t handle = SteamInput()->GetDigitalActionHandle(pszAction);
    if (handle != 0) {
        m_mpControllerDigitalActionHandles[unAction] = handle;
        if (bActionOnRelease) {
            m_mpActionWhenReleased[unAction] = true;
            m_mpActionIsPressed[unAction] = false;
        }
        return true;
    }

    return false;
}

bool EngineInput::RegisterAnalogAction(unsigned int unAction, const char* pszAction) {
    InputAnalogActionHandle_t handle = SteamInput()->GetAnalogActionHandle(pszAction);
    if (handle != 0) {
        m_mpControllerAnalogActionHandles[unAction] = handle;
        return true;
    }
    return false;
}

bool EngineInput::RegisterActionSet(unsigned int unActionSet, const char* pszActionSet) {
    InputActionSetHandle_t handle = SteamInput()->GetActionSetHandle(pszActionSet);
    if (handle != 0) {
        m_mpControllerActionSetHandles[unActionSet] = handle;
        return true;
    }
    return false;
}

void EngineInput::SetSteamControllerActionSet(unsigned int unActionSet) {
    if (m_hActiveController == 0)
        return;
    // This call is low-overhead and can be called repeatedly from game code that is active in a specific mode.
    if (m_mpControllerActionSetHandles.find(unActionSet) == m_mpControllerActionSetHandles.end()) {
#ifdef _DEBUG
        // Help to trace controller errors. To display in steamdeck, change project properties->linker->system from Windows to Console.
        // Then on steamdeck, press steammenu when running this 'game', select the console window, and press 'B' 
        if (gnActionSetError < 3 || (gnActionSetError % 1000) == 0) {
            std::cerr << "Cannot set action set handle, this has occured " << gnActionSetError << " times" << std::endl;
        }
        ++gnActionSetError;
#endif
        return;
    }
    SteamInput()->ActivateActionSet(m_hActiveController, m_mpControllerActionSetHandles[unActionSet]);
}

bool EngineInput::IsControllerActionActive(unsigned int unAction) {
    if (m_mpControllerDigitalActionHandles.find(unAction) == m_mpControllerDigitalActionHandles.end()) {
        return false;
    }
    ControllerDigitalActionData_t digitalData = SteamInput()->GetDigitalActionData(m_hActiveController, m_mpControllerDigitalActionHandles[unAction]);

    if (digitalData.bActive) {
        if (ActionIsOnRelease(unAction)) {
            if (ActionIsCurrentlyPressed(unAction)) {
                if (digitalData.bState) {
                    return false;
                }
                else {
                    SetActionIsCurrentlyPressed(unAction, false);
                    return true;
                }
            }
            else {
                if (digitalData.bState) {
                    SetActionIsCurrentlyPressed(unAction, true);
                }
                return false;
            }
        }
        else {
            return digitalData.bState;
        }
    }

    return false;
}

bool EngineInput::ActionIsOnRelease(unsigned int unAction) {
    auto find = m_mpActionWhenReleased.find(unAction);
    if (find == m_mpActionWhenReleased.end()) {
        return false;
    }
    return find->second;
}

bool EngineInput::ActionIsCurrentlyPressed(unsigned int unAction) {
    auto find = m_mpActionIsPressed.find(unAction);
    if (find == m_mpActionIsPressed.end()) {
        return false;
    }
    return find->second;
}

void EngineInput::SetActionIsCurrentlyPressed(unsigned int unAction, bool bPressed) {
    m_mpActionIsPressed[unAction] = bPressed;
}

void EngineInput::GetControllerAnalogAction(unsigned int unAction, float* pfX, float* pfY)
{
    if (m_mpControllerAnalogActionHandles.find(unAction) == m_mpControllerAnalogActionHandles.end()) {
        *pfX = 0.0f;
        *pfY = 0.0f;
        return;
    }
    InputDigitalActionHandle_t handle = m_mpControllerAnalogActionHandles[unAction];
    ControllerAnalogActionData_t analogData = SteamInput()->GetAnalogActionData(m_hActiveController, handle);

    // Actions are only 'active' when they're assigned to a control in an action set, and that action set is active.
    if (analogData.bActive)
    {
        *pfX = analogData.x;
        *pfY = analogData.y;
    }
    else
    {
        *pfX = 0.0f;
        *pfY = 0.0f;
    }
}

bool EngineInput::KeyPressed(unsigned int unKeyCode) {
    return m_pWnd->GetKey(unKeyCode) == GLFW_PRESS;
}

void EngineInput::ProcessMouseCallback(double fXPosIn, double fYPosIn) {
    m_pGameInput->ProcessMouseMovement((float)fXPosIn, (float)fYPosIn);
}

void EngineInput::ProcessScrollCallback(double fYOffset) {
    m_pGameInput->ProcessMouseScroll((float)fYOffset);
}

void EngineInput::ProcessKeyCallback(int nKey, int nScancode, int nAction, int nMods) {
    m_pGameInput->ProcessKeyEvent(nKey, nScancode, nAction, nMods);
}


void mouse_callback(GLFWwindow* pGLWnd, double xposIn, double yposIn)
{
    Window* pWindow = Window::FindAssociatedWindow(pGLWnd);
    if (pWindow == nullptr) {
        return;
    }
    EngineInput* pEngineInput = pWindow->GetEngineInput();
    pEngineInput->ProcessMouseCallback(xposIn, yposIn);
}

void scroll_callback(GLFWwindow* pGLWnd, double xoffset, double yoffset)
{
    Window* pWindow = Window::FindAssociatedWindow(pGLWnd);
    if (pWindow == nullptr) {
        return;
    }
    EngineInput* pEngineInput = pWindow->GetEngineInput();
    pEngineInput->ProcessScrollCallback(yoffset);
}

void key_callback(GLFWwindow* pGLWnd, int nKey, int nScancode, int nAction, int nMods) {
    Window* pWindow = Window::FindAssociatedWindow(pGLWnd);
    if (pWindow == nullptr) {
        return;
    }
    EngineInput* pEngineInput = pWindow->GetEngineInput();
    pEngineInput->ProcessKeyCallback(nKey, nScancode, nAction, nMods);
}

void focus_callback(GLFWwindow* pGLWnd, int focused) {
}