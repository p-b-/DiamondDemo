#include "GameLoop.h"
#include "Shader.h"
#include "Camera.h"
#include "EngineInput.h"
#include "GameInput.h"
#include "ShadowedScene.h"

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int gnFrameCount = 0;

GameLoop::GameLoop() {
    m_bGamePaused = false;
    m_pWnd = nullptr;
    m_pEngineInput = nullptr;
}

GameLoop::~GameLoop() {
    if (m_pEngineInput != nullptr) {
        m_pEngineInput->DeinitialiseAndDelete();
        m_pEngineInput = nullptr;
    }
}

void GameLoop::RunLoop(Window& wnd, Textures* pTextureCtrl, IScene* pScene) {
    pScene->SetGameLoop(this);
    m_pWnd = &wnd;
    // Note, in future versions, instead of IScene being passed into this method, a new Interface type IGame is passed.
    //  This provides the IScene and the IGameInput to the GameLoop. The engine's GameLoop (here) should not know about GameInput.

    GameInput* pGameInput = new GameInput(&wnd, pScene);

    m_pEngineInput = new EngineInput(&wnd, pGameInput);
    if (!m_pEngineInput->Initialise()) {
        wnd.ShouldClose(true);
    }
    else {
        // To display the control binding panel on start-up, uncomment this.
        /*if (SteamUtils()->IsSteamRunningOnSteamDeck()) {
            pIP->ShowBindingPanel();
        } */
    }

    bool bUsingShadows = pScene->GetUsingShadows();
    while (!wnd.IsClosing()) {
        gnFrameCount++;
        if (gnFrameCount == 100) {

        }
        wnd.StartRenderLoop(bUsingShadows);
        SteamAPI_RunCallbacks();
        float fDeltaTime = wnd.GetDeltaTime();
        if (m_pEngineInput != nullptr) {
            m_pEngineInput->ProcessInput(fDeltaTime);
        }
        pScene->AnimateScene(fDeltaTime);

        if (bUsingShadows) {
            pScene->DrawScene(true);
            m_pWnd->EndShadowRendering();
        }
        pScene->DrawScene(false);

        wnd.EndRenderLoop();
        wnd.SwapBuffers();
    }
    pScene->Deinitialise();
    Entity::ClearUnreferencedEntities();

    if (m_pEngineInput != nullptr) {
        m_pEngineInput->DeinitialiseAndDelete();
        m_pEngineInput = nullptr;
    }
    delete pGameInput;
    pGameInput = nullptr;
}

void GameLoop::PauseGame(bool bPause) {
    m_bGamePaused = bPause;
}

void GameLoop::QuitGame() {
    m_pWnd->ShouldClose(true);
}

bool GameLoop::IsPaused() {
    return m_bGamePaused;
}

void GameLoop::DisplayDepthMap(bool bShow, unsigned int unDepthMapTexture, float fNearPlane, float fFarPlane) {
    m_pWnd->ShowDepthMap(bShow, unDepthMapTexture, fNearPlane, fFarPlane);
}

void GameLoop::DisplayBindingPanel() {
    m_pEngineInput->ShowBindingPanel();
}
