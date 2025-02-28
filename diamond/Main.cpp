#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <Windows.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <iostream>
#include <steam/steam_api.h>

#include "Textures.h"
#include "Window.h"
#include "DiamondScene.h"

#include "Shader.h"
#include "GameLoop.h"

bool InitialiseSteam();
void DeinitialiseSteam();
extern "C" void __cdecl SteamAPIDebugTextHook(int nSeverity, const char* pchDebugText);
int Alert(const WCHAR* lpCaption, const WCHAR* lpText);

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 800;

int main();

int APIENTRY WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nCmdShow) {
    return main();
}

int main()
{
    // If running with a console, and do not want to show it on steamdeck two windows interferes with controller focus, call this.
    // ::ShowWindow(::GetConsoleWindow(), SW_HIDE);
    
    if (!InitialiseSteam()) {
        return EXIT_FAILURE;
    }

    Window wnd;
    if (!wnd.Initialise(SCR_WIDTH, SCR_HEIGHT)) {
        DeinitialiseSteam();
        return EXIT_FAILURE;
    }
    wnd.SetClearColour(0.0f, 0.0f, 0.0f, 1.0f);

    Textures textureCtrl;

    DiamondScene* pScene = new DiamondScene();
    pScene->Initialise(&textureCtrl, &wnd);
    bool bUsingShadows = pScene->GetUsingShadows();

    GameLoop loop;
    loop.RunLoop(wnd, bUsingShadows, &textureCtrl, pScene);

    wnd.Deinitialise();
    DeinitialiseSteam();
        
    return EXIT_SUCCESS;
}

extern "C" void __cdecl SteamAPIDebugTextHook(int nSeverity, const char* pchDebugText)
{
    std::cout << "Severity: " << nSeverity << " msg: " << pchDebugText << std::endl;
    // if you're running in the debugger, only warnings (nSeverity >= 1) will be sent
    // if you add -debug_steamapi to the command-line, a lot of extra informational messages will also be sent
    ::OutputDebugStringA(pchDebugText);
    
    if (nSeverity >= 1)
    {
        // place to set a breakpoint for catching API errors
        int x = 3;
        (void)x;
    }
}

int Alert(const WCHAR* lpCaption, const WCHAR* lpText)
{
#ifndef _WIN32
    fprintf(stderr, "Message: '%s', Detail: '%s'\n", lpCaption, lpText);
    return 0;
#else
    return ::MessageBox(NULL, lpText, lpCaption, MB_OK);
#endif
}

bool InitialiseSteam() {
    if (SteamAPI_RestartAppIfNecessary(480)) // Replace with your App ID
    {
        std::cout << "SteamAPI_RestartAppIfNecessary() returned false " << std::endl;
        return false;
    }

    SteamErrMsg errMsg = { 0 };
    bool steamInitialised = false;
    if (SteamAPI_InitEx(&errMsg) != k_ESteamAPIInitResult_OK)
    {
        OutputDebugStringA("SteamAPI_Init() failed: ");
        OutputDebugStringA(errMsg);
        OutputDebugStringA("\n");

        Alert(L"Fatal Error", L"Steam must be running to play this game (SteamAPI_Init() failed).\n");
        return false;
    }
    else {
        steamInitialised = true;
    }

    if (!SteamUser()->BLoggedOn())
    {
        OutputDebugStringA("Steam user is not logged in\n");
        //Alert(L"Fatal Error", L"Steam user must be logged in to play this game (SteamUser()->BLoggedOn() returned false).\n");
        //return false;
    }
    SteamClient()->SetWarningMessageHook(&SteamAPIDebugTextHook);

    // restrict this main game thread to the first processor, so query performance counter won't jump on crappy AMD cpus
    DWORD dwThreadAffinityMask = 0x01;
    ::SetThreadAffinityMask(::GetCurrentThread(), dwThreadAffinityMask);
    return true;
}

void DeinitialiseSteam() {
    SteamAPI_Shutdown();
}
