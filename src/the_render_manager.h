#pragma once

class TheRenderManager {
    public:
        static TheRenderManager* Instance();

    private:
        TheRenderManager(){};
        TheRenderManager(TheRenderManager const&) = delete;
        TheRenderManager& operator=(TheRenderManager const&) = delete;

        static TheRenderManager* instance;
};
