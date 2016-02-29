#pragma once

#include "camera.h"
#include "scene.h"

enum class RenderType {
    phong,
    voxelPoints,
    directvolume,
    raytrace
};

class TheRenderManager {
    public:
        static TheRenderManager* Instance();

        void Init(int width, int height);

        void Set_scene(Scene scene);
        void Set_render_size(int width, int height);

        void Voxelize();
        void Render(Camera* camera);
        void Render_framebuffer();
        void Raytrace(Camera* camera, bool full);

        void Use_defered();
        void Init_voxelization(int resolution);
        void Init_raytrace();

        RenderType currentRenderer;

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

        uint64_t raytraceTex;

        int voxelResolution;
        GLuint voxelColor, voxelNorm;

        glm::mat4 xorth, yorth, zorth;

        bool defered = false;
        bool raytrace = false;

        GLuint voxelVAO, voxelPos;
};
