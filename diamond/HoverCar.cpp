#include "HoverCar.h"
#include "Textures.h"
#include "Camera.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>


HoverCar::HoverCar() {
    m_unEBO = 0;
    m_unVAO = 0;
    m_unVBO = 0;

    fColourR = 0.0f;
    fColourG = 0.8f;
    fColourB = 0.1f;
}

HoverCar::~HoverCar() {

}

void HoverCar::Initialise(unsigned int unEntityId) {
    Initialise(unEntityId, nullptr);
}

void HoverCar::Initialise(unsigned int unEntityId, Textures* pTextureCtrl) {
    m_unEntityId = unEntityId;

    // Create source vertices, that are just the points of the polygon
    CreateSourceVertices();
    CalculateBoundingSphereFromSourceVertices();

    // These vertices are then replicated and given normals, to construct the vertices passed to gl

    float* verticesForGL = CreateGLTriVerticesFromSourceData();

    glGenVertexArrays(1, &m_unVAO);
    glGenBuffers(1, &m_unVBO);

    // The Vertex Array Object stores the state associated with the VBO, so the following code is stored, ensuring
    //  the binding of the vbo, and the setup of the vertex data, does not need to be repeated
    glBindVertexArray(m_unVAO);

    // The Vertex Buffer Object is of type GL_ARRAY_BUFFER. This binds the VBO identified by m_unVBO so that
    //  future operations on GL_ARRAY_BUFFER are performed on this VBO.
    glBindBuffer(GL_ARRAY_BUFFER, m_unVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 3 * m_unTriCount, verticesForGL, GL_STATIC_DRAW);

    unsigned int unStride = 6 * sizeof(float);
    unsigned long long unOffset = 0 * sizeof(float);
    unsigned int unPosSize = 3;
    unsigned int unNormalSize = 3;
    unsigned int unTextureSize = 2;

    bool bHasTexture = false;
    int attribArrayIndex = 0;
    // position attribute
    glVertexAttribPointer(attribArrayIndex, unPosSize, GL_FLOAT, GL_FALSE, unStride, (void*)unOffset);
    glEnableVertexAttribArray(attribArrayIndex++);
    unOffset += unPosSize * sizeof(float);

    // normal attribute
    glVertexAttribPointer(attribArrayIndex, unNormalSize, GL_FLOAT, GL_FALSE, unStride, (void*)unOffset);
    glEnableVertexAttribArray(attribArrayIndex++);
    unOffset += unNormalSize * sizeof(float);

    glBindVertexArray(0);
    if (bHasTexture) {
        // texture coord attribute
        glVertexAttribPointer(attribArrayIndex, unTextureSize, GL_FLOAT, GL_FALSE, unStride, (void*)unOffset);
        glEnableVertexAttribArray(attribArrayIndex++);
        unOffset += unTextureSize * sizeof(float);
    }

    m_shader.InitialiseShaders("polyhedral_vert.glsl", "polyhedral_frag.glsl");
    m_shaderShadow.InitialiseShaders("polyhedral_shadow_vert.glsl", "polyhedral_shadow_frag.glsl");
    m_shader.Use();
    m_shader.SetInt("shadowMap", 0);

//    m_shader.InitialiseShaders("hovercar.vs", "hovercar.fs");

    DeleteSrcData();

    delete[] verticesForGL;
}

void HoverCar::CreateSourceVertices() {
    InitialiseSrcData();
    SetNextVertex(0.4f, -0.2f,  0.5f);
    SetNextVertex(0.4f, -0.2f, -0.5f);
    SetNextVertex( -0.4f, -0.2f, -0.5f);
    SetNextVertex( -0.4f, -0.2f,  0.5f);
    
    SetNextVertex(0.2f,  0.2f,  0.3f);
    SetNextVertex( 0.2f,  0.2f, -0.1f);
    SetNextVertex( -0.2f,  0.2f, -0.1f);
    SetNextVertex( -0.2f,  0.2f,  0.3f);

    // Bottom
    CreateNextSrcTriangle(0, 2, 1);
    CreateNextSrcTriangle(0, 3, 2);
    // Top
    CreateNextSrcTriangle(4, 5, 6);
    CreateNextSrcTriangle(4, 6, 7);
    // Rear
    CreateNextSrcTriangle(0, 4, 7);
    CreateNextSrcTriangle(0, 7, 3);
    // Right side
    CreateNextSrcTriangle(0, 5, 4);
    CreateNextSrcTriangle(0, 1, 5);
    // Left side
    CreateNextSrcTriangle(3, 7, 6);
    CreateNextSrcTriangle(3, 6, 2);
    // Front
    CreateNextSrcTriangle(2, 6, 5);
    CreateNextSrcTriangle(2, 5, 1);
}

void HoverCar::Deinitialise() {
    glDeleteVertexArrays(1, &m_unVAO);
    m_unVAO = 0;
    glDeleteBuffers(1, &m_unVBO);
    m_unVBO = 0;

}

void HoverCar::PrepareToDraw(Camera* pCamera, const glm::vec3& vecLightPos, const glm::mat4& matLightSpace, unsigned int unDepthMapTexture) {
    glBindVertexArray(m_unVAO);
    m_shader.Use();
    m_shader.SetMat4("lightSpaceTransform", matLightSpace);
    m_shader.SetVec3("objectColor", fColourR, fColourG, fColourB);
    m_shader.SetVec3("lightColor", 1.0f, 1.0f, 1.0f);
    m_shader.SetVec3("lightPos", vecLightPos);
    m_shader.SetVec3("viewPos", pCamera->GetPosition());

    m_shader.SetMat4("projection", pCamera->GetProjectionMatrix());
    m_shader.SetMat4("view", pCamera->GetViewMatrix());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, unDepthMapTexture);
}

void HoverCar::SetColour(float r, float g, float b) {
    fColourR = r;
    fColourG = g;
    fColourB = b;
}