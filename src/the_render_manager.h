#pragma once

#include "camera.h"
#include "scene.h"

class TheRenderManager {
    enum Voxels {
        postition, color, normal, AMOUNT
    };

    public:
        static TheRenderManager* Instance();

        void Init(int width, int height);

        void Set_scene(Scene scene);
        void Set_render_size(int width, int height);

        void Voxelize(int resolution);
        void Render(Camera* camera);
        void Render_framebuffer();

        void Use_defered();
        void Init_voxelization();

    private:
        TheRenderManager(){};
        TheRenderManager(TheRenderManager const&) = delete;
        TheRenderManager& operator=(TheRenderManager const&) = delete;

        static TheRenderManager* instance;

        const static GLfloat screenQuad[];

        Scene currentScene;
        int renderWidth, renderHeight;
        GLuint voxelCount;
        std::vector<uint64_t> voxels;

        GLuint framebuffer, quadVA, quadbuffer;

        uint64_t* framebufferTextures;

        bool defered = false;
};
