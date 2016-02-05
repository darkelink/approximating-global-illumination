#pragma once

#include <GL/glew.h>
#include <GL/gl.h>

#include <map>
#include <stdint.h>
#include <string>
#include <vector>

namespace GBuffer {
    enum Target : int {
        color,
        normal,
        AMOUNT
    };
}

class TheTextureManager {
    public:
        static TheTextureManager* Instance();

        GLuint Create_empty(int width, int height);
        uint64_t Create_from_file(std::string filename);

        void Delete(uint64_t handle);

        GLuint Create_framebuffer(int width, int height);
        GLuint Resize_framebuffer(int width, int height);
        std::vector<uint64_t> Get_framebuffer_textures();

    private:
        TheTextureManager(){};
        TheTextureManager(TheTextureManager const&) = delete;
        TheTextureManager& operator=(TheTextureManager const&) = delete;
        static TheTextureManager* instance;

        static const GLenum drawBuffers[GBuffer::AMOUNT];

        std::map<std::string, uint64_t> files;
        std::map<uint64_t, GLuint> textures;

        GLuint framebuffer;
        std::vector<uint64_t> gBufferTextures;
        std::vector<GLuint> gBuffer;
        GLuint depth;

        uint64_t make_bindless(GLuint textureID);
};
