#pragma once

#include "shader.h"

#include <GL/glew.h>

#include <string>
#include <vector>

namespace Shaders {
    enum ShaderType : int {
        basic,
        AMOUNT
    };

}

class TheShaderManager {
    public:
        static TheShaderManager* Instance();

        void Load_all();
        void Reload(Shaders::ShaderType type);
        void Use(Shaders::ShaderType type);

        void Set_uniform(std::string, glm::mat4 data);

    private:
        TheShaderManager(){};
        TheShaderManager(TheShaderManager const&) = delete;
        TheShaderManager& operator=(TheShaderManager const&) = delete;

        std::string load_file(std::string filename);

        static TheShaderManager* instance;

        std::vector<Shader*> shaders;
        Shaders::ShaderType current;
};
