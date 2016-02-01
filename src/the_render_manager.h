#pragma once

#include "camera.h"
#include "scene.h"

class TheRenderManager {
    public:
        static TheRenderManager* Instance();

        void Init();

        void Set_scene(Scene scene);
        void Set_render_size(int width, int height);

        void Voxelize(int size);
        void Render(Camera* camera);

    private:
        TheRenderManager(){};
        TheRenderManager(TheRenderManager const&) = delete;
        TheRenderManager& operator=(TheRenderManager const&) = delete;

        static TheRenderManager* instance;

        Scene currentScene;
        int renderWidth, renderHeight;
        GLuint voxelCount;
};
