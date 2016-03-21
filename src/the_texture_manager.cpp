#include "the_texture_manager.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <fstream>
#include <iostream>

static inline bool file_exists(std::string filename) {
    std::ifstream infile(filename.c_str());
    return infile.good();
}

const GLenum TheTextureManager::drawBuffers[GBuffer::AMOUNT] = {
    GL_COLOR_ATTACHMENT0,
    GL_COLOR_ATTACHMENT1
};

TheTextureManager* TheTextureManager::instance = 0;

TheTextureManager* TheTextureManager::Instance() {
    if (!instance) {
        instance = new TheTextureManager;
    }
    return instance;
}

uint64_t TheTextureManager::make_bindless(GLuint textureID) {
    uint64_t handle = glGetTextureHandleARB(textureID);
    glMakeTextureHandleResidentARB(handle);
    textures.insert(std::make_pair(handle, textureID));
    return handle;
}

uint64_t TheTextureManager::Create_empty(std::vector<int> dimentions, GLenum format) {
    GLuint textureID;
    glGenTextures(1, &textureID);

    switch (dimentions.size()) {
        case 1:
            if (texBuffer <= 0) {
                glGenBuffers(1, &texBuffer);
                glBindBuffer(GL_TEXTURE_BUFFER, texBuffer);
                // assume all 1D data is the same size
                glBufferData(GL_TEXTURE_BUFFER, dimentions[0], 0, GL_STATIC_DRAW);
            }
            glBindTexture(GL_TEXTURE_BUFFER, textureID);
            glTexBuffer(GL_TEXTURE_BUFFER, format, texBuffer);
            break;
        case 2:
            glBindTexture(GL_TEXTURE_2D, textureID);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

            glTexImage2D(GL_TEXTURE_2D, 0, format, dimentions[0], dimentions[1], 0,
                    format, GL_UNSIGNED_BYTE, nullptr);
            break;
        case 3:
            glBindTexture(GL_TEXTURE_3D, textureID);

            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_BASE_LEVEL, 0);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, 0);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            glTexImage3D(GL_TEXTURE_3D, 0, format,
                    dimentions[0], dimentions[1], dimentions[2], 0,
                    format, GL_UNSIGNED_BYTE, nullptr);
            break;
    }

    return make_bindless(textureID);
}

uint64_t TheTextureManager::Create_from_file(std::string filename) {
    if (!file_exists(filename)) {
        filename = "res/models/textures/missing.tga";
    }

    auto exists = files.find(filename);
    if (exists != files.end()) {
        // texture already exists
        return exists->second;
    }
    //std::cout << filename << std::endl;

    GLuint textureID;
    glGenTextures(1, &textureID);

    unsigned char* image;
    int width, height, components;
    image = stbi_load(filename.c_str(), &width, &height, &components, 0);

    glBindTexture(GL_TEXTURE_2D, textureID);
    if (components == 3) {
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, width, height);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height,
                GL_RGB, GL_UNSIGNED_BYTE, image);
    } else {// if (components == 4) {
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height,
                GL_RGBA, GL_UNSIGNED_BYTE, image);
    }

    uint64_t handle = make_bindless(textureID);
    files.insert(std::make_pair(filename, handle));

    stbi_image_free(image);

    return handle;
}

void TheTextureManager::Delete(uint64_t handle) {
    auto exists = textures.find(handle);
    if (exists != textures.end()) {
        glMakeTextureHandleNonResidentARB(handle);
        glDeleteTextures(1, &exists->second);
        textures.erase(exists);
    }
}

GLuint TheTextureManager::Create_framebuffer(int width, int height) {
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    gBuffer.reserve(GBuffer::AMOUNT);
    gBufferTextures.reserve(GBuffer::AMOUNT);
    for (int i = 0; i < GBuffer::AMOUNT; ++i) {
        gBufferTextures.push_back(Create_empty({width, height}, GL_RGB));
        gBuffer.push_back(textures[gBufferTextures[i]]);
    }

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            gBuffer[GBuffer::color], 0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
            gBuffer[GBuffer::normal], 0);

     glGenRenderbuffers(1, &depth);
     glBindRenderbuffer(GL_RENDERBUFFER, depth);
     glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
     glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
            GL_RENDERBUFFER, depth);

    glDrawBuffers(GBuffer::AMOUNT, drawBuffers);
    return framebuffer;
}

GLuint TheTextureManager::Resize_framebuffer(int width, int height) {
    // resizing framebuffers rearly works, delete and recreate instead
    for (auto tex : gBufferTextures) {
        Delete(tex);
    }
    gBufferTextures.clear();
    gBuffer.clear();

    glDeleteRenderbuffers(1, &depth);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &framebuffer);

    return Create_framebuffer(width, height);
}

std::vector<uint64_t> TheTextureManager::Get_framebuffer_textures() {
    return gBufferTextures;
}

GLuint TheTextureManager::Create_voxel_store(int resolution) {
    // cannot use bindless textures here
    GLuint textureID;
    glGenTextures(1, &textureID);

    glBindTexture(GL_TEXTURE_3D, textureID);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    std::vector<unsigned int> zero(resolution*resolution*resolution, 0);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_R32UI, resolution, resolution, resolution, 0,
            GL_RED_INTEGER, GL_UNSIGNED_INT, &zero.front());

    return textureID;
}

GLuint TheTextureManager::GetID(uint64_t tex) {
    return textures[tex];
}
