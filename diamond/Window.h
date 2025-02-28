#pragma once
#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "EngineInput.h"

class Shader;

class Window
{
public:
	Window();
	~Window();
	bool Initialise(int width, int height);
	void Deinitialise();
	void SetMouseCallback(GLFWcursorposfun func);
	void SetScrollCallback(GLFWscrollfun func);
	void SetKeyCallback(GLFWkeyfun func);
	void SetFocusCallback(GLFWwindowfocusfun func);


	void CaptureMouse();
	void SetClearColour(float red, float green, float blue, float alpha);

	void StartRenderLoop(bool bHasShadows);
	void EndRenderLoop();
	void EndShadowRendering();

	bool IsClosing();
	int GetKey(int key);
	void ShouldClose(bool shouldClose);
	void SwapBuffers();
	static void FramebufferSizeCallback(GLFWwindow* pGLWnd, int width, int height);
	static Window* FindAssociatedWindow(GLFWwindow* pGLWnd);

	bool IsFocused();
	void FocusToWindow(bool focus);

public:
	int GetWidth() const { return m_nWidth; }
	int GetHeight() const { return m_nHeight; }
	float GetDeltaTime() const { return m_fDeltaTime; }
	void SetInput(EngineInput* pEngineInput) { m_pEngineInput = pEngineInput; }
	EngineInput* GetEngineInput() { return m_pEngineInput; }
	void ShowDepthMap(bool bShow, unsigned int unDepthMapTexture, float fNearPlane, float fFarPlane);

protected:
	void RemoveWnd();
	void RenderDepthMapToScreen();
	void InitialiseDepthMapDisplay();

private:
	int m_nWidth;
	int m_nHeight;
	bool m_bClosing;

	float m_fClearRed;
	float m_fClearGreen;
	float m_fClearBlue;
	float m_fClearAlpha;

	GLFWwindow* m_pWindow;
	static std::vector<Window* > m_svcWnds;

	float m_fDeltaTime = 0.0f;	// time between current frame and last frame
	float m_fLastFrame = 0.0f;

	EngineInput* m_pEngineInput;

	bool m_bShowDepthMap;
	unsigned int m_unDepthMapVAO;
	unsigned int m_unDepthMapVBO;
	unsigned int m_unDepthMapTexture;
	float m_fNearPlane;
	float m_fFarPlane;
	Shader* m_pShaderDepthMap;
};

