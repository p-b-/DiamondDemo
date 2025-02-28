#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>

#include "Textures.h"

Textures::Textures() {
}

unsigned int Textures::CreateTexture(const char* pzFilepath, const std::string& name, bool useAlpha, bool flipVertically) {
    unsigned int textureId = GetTextureIdFromName(name);
    if (textureId != 0) {
        return textureId;
    }
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    if (flipVertically) {
        stbi_set_flip_vertically_on_load(true); 
    }
    unsigned char* data = stbi_load(pzFilepath, &width, &height, &nrChannels, 0);
    if (flipVertically) {
        stbi_set_flip_vertically_on_load(false);
    }
    if (data)
    {
        unsigned int colourFormat = GL_RGB;
        if (useAlpha) {
            colourFormat = GL_RGBA;
        }
        glTexImage2D(GL_TEXTURE_2D, 0, colourFormat, width, height, 0, colourFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cerr << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    m_mapTextureNameToId[name] = textureId;

    return textureId;
}

unsigned int Textures::GetTextureIdFromName(const std::string& name) {
    auto iter = m_mapTextureNameToId.find(name);
    if (iter == m_mapTextureNameToId.end()) {
        return 0;
    }
    return iter->second;
}