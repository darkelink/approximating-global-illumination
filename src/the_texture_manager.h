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

        uint64_t Create_empty(std::vector<int> dimentions, GLenum format);
        uint64_t Create_from_file(std::string filename);

        void Delete(uint64_t handle);

        GLuint Create_framebuffer(int width, int height);
        GLuint Resize_framebuffer(int width, int height);
        std::vector<uint64_t> Get_framebuffer_textures();

        GLuint Create_voxel_store(int resolution);

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

        // use a buffer for 1D textures
        GLuint texBuffer;

        uint64_t make_bindless(GLuint textureID);
};
