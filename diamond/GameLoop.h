#pragma once

#include "IGameLoop.h"
#include "Window.h"
#include "Textures.h"
#include "Shader.h"
#include "IScene.h"
#include <map>
#include "IEngineInputIntl.h"

class GameLoop : public IGameLoop
{
public:
	GameLoop();
	~GameLoop();
	GameLoop(const GameLoop& toCopy) = delete;
	GameLoop(GameLoop&& toMove) noexcept = delete;

	void RunLoop(Window& wnd, bool bUseShadows, Textures* pTextureCtrl, IScene* pScene);

public:
	virtual void PauseGame(bool bPause);
	virtual void QuitGame();
	virtual bool IsPaused();
	virtual void DisplayDepthMap(bool bShow, unsigned int unDepthMapTexture, float fNearPlane, float fFarPlane);
	virtual void DisplayBindingPanel();

private:
	Window* m_pWnd;
	std::map<unsigned int, Shader*> m_mapShaderById;
	bool m_bGamePaused;
	IEngineInputIntl* m_pEngineInput;
};

