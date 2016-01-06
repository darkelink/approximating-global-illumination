#include "the_shader_manager.h"

#include <fstream>
#include <map>

#include <iostream>

TheShaderManager* TheShaderManager::instance = 0;

// define each shader by its file names
// make sure they are in the same order as the enum
std::vector<std::map<int, std::string> > shaderDefs = {
    {
        {GL_VERTEX_SHADER, "basic.vert.glsl"},
        {GL_FRAGMENT_SHADER, "basic.frag.glsl"}
    }
};


TheShaderManager* TheShaderManager::Instance() {
    if (!instance) {
        instance = new TheShaderManager;
        instance->shaders.reserve(Shaders::AMOUNT);
    }
    return instance;
}


std::string TheShaderManager::load_file(std::string filename) {
    std::ifstream t("res/shaders/" + filename);
    std::string str((std::istreambuf_iterator<char>(t)),
                 std::istreambuf_iterator<char>());
    return str;
}


void TheShaderManager::Load_all() {
    // an iterator would include AMOUNT
    for (int i = 0; i < Shaders::AMOUNT; ++i) {
        Reload(static_cast<Shaders::ShaderType>(i));
    }
}


void TheShaderManager::Reload(Shaders::ShaderType type) {
    // remove old shader
    if (shaders[type]->Ready) {
        shaders[type]->Delete();
    }

    Shader* s = new Shader();

    for (auto sh : shaderDefs[type]) {
        s->Load(load_file(sh.second), sh.first);
    }
    s->Link();

    shaders[type] = s;
}


void TheShaderManager::Use(Shaders::ShaderType type) {
    shaders[type]->Use();
    current = type;
}


void TheShaderManager::Set_uniform(std::string name, glm::mat4 data) {
    shaders[current]->Set_uniformm4fv(name.c_str(), data);
}