#pragma once

#include "shader.h"

#include <GL/glew.h>

#include <string>
#include <vector>

namespace Shaders {
    enum ShaderType : int {
        shaded,
        defered,
        composite,
        screenquad,
        voxelize,
        drawPoints,
        dvr,
        raytrace,
        AMOUNT
    };
}

enum class Uniform {
    mat4, i1, f1, vec3
};

class TheShaderManager {
    public:
        static TheShaderManager* Instance();

        void Load_all();
        void Reload(Shaders::ShaderType type);
        void Use(Shaders::ShaderType type);

        void Set_uniform(Uniform type, std::string name, void* data);

        void Get_info(GLenum info, GLint* params);

    private:
        TheShaderManager(){};
        TheShaderManager(TheShaderManager const&) = delete;
        TheShaderManager& operator=(TheShaderManager const&) = delete;

        std::string load_file(std::string filename);

        static TheShaderManager* instance;

        std::vector<Shader*> shaders;
        Shaders::ShaderType current;
};
