#pragma once

#include "camera.h"
#include "scene.h"

class TheRenderManager {
    public:
        static TheRenderManager* Instance();

        void Init(int width, int height);

        void Set_scene(Scene scene);
        void Set_render_size(int width, int height);

        void Voxelize();
        void Render(Camera* camera);
        void Render_voxels(Camera* camera);
        void Render_framebuffer();

        void Use_defered();
        void Init_voxelization(int resolution);

    private:
        TheRenderManager(){};
        TheRenderManager(TheRenderManager const&) = delete;
        TheRenderManager& operator=(TheRenderManager const&) = delete;

        static TheRenderManager* instance;

        const static GLfloat screenQuad[];

        Scene currentScene;
        int renderWidth, renderHeight;

        GLuint framebuffer, quadVA, quadbuffer;

        uint64_t* framebufferTextures;

        int voxelResolution;
        GLuint voxels;

        bool defered = false;

        GLuint voxelVAO, voxelPos;
};
