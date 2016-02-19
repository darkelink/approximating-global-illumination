#pragma once

#include "shader.h"

#include <GL/glew.h>

#include <string>
#include <vector>

namespace Shaders {
    enum ShaderType : int {
        shaded,
        defered,
        screen,
        voxelize,
        drawVoxels,
        drawPoints,
        AMOUNT
    };
}

enum class Uniform {
    mat4, i1, f1
};

class TheShaderManager {
    public:
        static TheShaderManager* Instance();

        void Load_all();
        void Reload(Shaders::ShaderType type);
        void Use(Shaders::ShaderType type);

        void Set_uniform(Uniform type, std::string name, void* data);

    private:
        TheShaderManager(){};
        TheShaderManager(TheShaderManager const&) = delete;
        TheShaderManager& operator=(TheShaderManager const&) = delete;

        std::string load_file(std::string filename);

        static TheShaderManager* instance;

        std::vector<Shader*> shaders;
        Shaders::ShaderType current;
};
