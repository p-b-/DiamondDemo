#include <iostream>

#include "Window.h"
#include "Shader.h"

std::vector<Window* > Window::m_svcWnds;

void errorCallback(int error, const char* description)
{
    std::cout << "ERR: " << description << std::endl;
    fprintf(stderr, "Error: %s\n", description);
}

Window::Window() {
    m_nWidth= 0;
    m_nHeight= 0;
    m_bClosing= 0;

    m_fClearRed= 0;
    m_fClearGreen= 0;
    m_fClearBlue= 0;
    m_fClearAlpha= 0;

    m_pWindow = nullptr;
    m_pEngineInput = nullptr;

    m_pShaderDepthMap = nullptr;
    m_unDepthMapTexture = 0;
    m_fNearPlane = 1.0f;
    m_fFarPlane = 7.5f;
}

Window::~Window() {
    delete m_pShaderDepthMap;
    m_pShaderDepthMap = nullptr;
}

bool Window::Initialise(int width, int height) {
    m_nWidth = width;
    m_nHeight = height;
    m_bClosing = false;

    // glfw: initialize and configure
// ------------------------------
    if (!glfwInit()) {
        return false;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
    glfwWindowHint(GLFW_CONTEXT_DEBUG, GLFW_TRUE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    m_pWindow = glfwCreateWindow(m_nWidth, m_nHeight, "Diamond", NULL, NULL);
    if (m_pWindow == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwSetErrorCallback(errorCallback);
    glfwMakeContextCurrent(m_pWindow);
    glfwSetFramebufferSizeCallback(m_pWindow, FramebufferSizeCallback);

    if (!gladLoadGL()) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    Window::m_svcWnds.push_back(this);
    return true;
}

bool Window::IsFocused() {
    int focused = glfwGetWindowAttrib(m_pWindow, GLFW_FOCUSED);
    return focused == 1;
}

void Window::FocusToWindow(bool focus) {
    if (focus) {
        glfwFocusWindow(m_pWindow);
    }
    else {
    }
}

void Window::Deinitialise() {
    glfwTerminate();
}

void Window::SetMouseCallback(GLFWcursorposfun func) {
    glfwSetCursorPosCallback(m_pWindow, func);

}

void Window::SetScrollCallback(GLFWscrollfun func) {
    glfwSetScrollCallback(m_pWindow, func);
}

void Window::SetKeyCallback(GLFWkeyfun func) {
    glfwSetKeyCallback(m_pWindow, func);
}

void Window::SetFocusCallback(GLFWwindowfocusfun func) {
    glfwSetWindowFocusCallback(m_pWindow, func);
}


void Window::CaptureMouse() {
    glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Window::SetClearColour(float red, float green, float blue, float alpha) {
    m_fClearRed = red;
    m_fClearGreen = green;
    m_fClearBlue = blue;
    m_fClearAlpha = alpha;
}

void Window::StartRenderLoop(bool bHasShadows) {
    float currentFrame = static_cast<float>(glfwGetTime());
    m_fDeltaTime = currentFrame - m_fLastFrame;
    m_fLastFrame = currentFrame;
    
    if (!bHasShadows) {
        glClearColor(m_fClearRed, m_fClearGreen, m_fClearBlue, m_fClearAlpha);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
}

void Window::EndShadowRendering() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_nWidth, m_nHeight);
    glClearColor(m_fClearRed, m_fClearGreen, m_fClearBlue, m_fClearAlpha);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void Window::EndRenderLoop() {
    if (m_bShowDepthMap) {
        RenderDepthMapToScreen();
    }
    glfwPollEvents();
}

void Window::RemoveWnd() {
    // TODO Should lock this at al access points, however, it is unlikely to have multiple windows right now
    for (std::vector<Window* >::iterator iter = m_svcWnds.begin(); iter != m_svcWnds.end(); iter++) {
        if ((*iter) == this) {
            m_svcWnds.erase(iter);
            break;
        }
    }
}

bool Window::IsClosing() {
    return glfwWindowShouldClose(m_pWindow) || m_bClosing;
}

int Window::GetKey(int key) {
    return glfwGetKey(m_pWindow, key);
}

void Window::ShouldClose(bool shouldClose) {
    glfwSetWindowShouldClose(m_pWindow, shouldClose);
    m_bClosing = true;
}

void Window::SwapBuffers() {
    glfwSwapBuffers(m_pWindow);
}

void Window::FramebufferSizeCallback(GLFWwindow* pGLWnd, int width, int height) {
    std::cout << "Framebuffer size changed to " << width << "x" << height << std::endl;
    Window* pWnd = FindAssociatedWindow(pGLWnd);
    if (pWnd != nullptr) {
        pWnd->m_nWidth = width;
        pWnd->m_nHeight = height;
    }
    glViewport(0, 0, width, height);
}

Window* Window::FindAssociatedWindow(GLFWwindow* pGLWnd) {
    for (Window* pWnd : m_svcWnds) {
        if (pWnd->m_pWindow == pGLWnd) {
            return pWnd;
        }
    }
    return nullptr;
}

void Window::ShowDepthMap(bool bShow, unsigned int unDepthMapTexture, float fNearPlane, float fFarPlane) {
    m_bShowDepthMap = bShow;
    m_unDepthMapTexture = unDepthMapTexture;
    m_fNearPlane = fNearPlane;
    m_fFarPlane = fFarPlane;
}

void Window::RenderDepthMapToScreen() {
    if (m_unDepthMapVAO == 0) {
        InitialiseDepthMapDisplay();
    }

    m_pShaderDepthMap->Use();
    m_pShaderDepthMap->SetFloat("near_plane", m_fNearPlane);
    m_pShaderDepthMap->SetFloat("far_plane", m_fFarPlane);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_unDepthMapTexture);

    glBindVertexArray(m_unDepthMapVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void Window::InitialiseDepthMapDisplay() {
    float rectangleVertices[] = {
        -1.0f,  1.0f, 0.0f,   0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f,   0.0f, 0.0f,
         1.0f,  1.0f, 0.0f,   1.0f, 1.0f,
         1.0f, -1.0f, 0.0f,   1.0f, 0.0f,
    };

    glGenVertexArrays(1, &m_unDepthMapVAO);
    glGenBuffers(1, &m_unDepthMapVBO);
    glBindVertexArray(m_unDepthMapVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_unDepthMapVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), &rectangleVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    m_pShaderDepthMap = new Shader();
    m_pShaderDepthMap->InitialiseShaders("depth_display_vert.glsl", "depth_display_frag.glsl");
    m_pShaderDepthMap->Use();
    m_pShaderDepthMap->SetInt("depthMap", 0);
}