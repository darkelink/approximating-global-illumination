#pragma once

#include "scene.h"

class TheRenderManager {
    public:
        static TheRenderManager* Instance();

        void Set_scene(Scene scene);
        void Set_render_size(int width, int height);

        void Voxelize(int size);
        void Render();

    private:
        TheRenderManager(){};
        TheRenderManager(TheRenderManager const&) = delete;
        TheRenderManager& operator=(TheRenderManager const&) = delete;

        static TheRenderManager* instance;

        Scene currentScene;
        int renderWidth, renderHeight;
};
