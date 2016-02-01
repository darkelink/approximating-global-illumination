#include "the_render_manager.h"

#include "the_shader_manager.h"

#include <glm/gtc/type_ptr.hpp>

#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>

TheRenderManager* TheRenderManager::instance = 0;

TheRenderManager* TheRenderManager::Instance() {
    if (!instance) {
        instance = new TheRenderManager;
    }
    return instance;
}

void TheRenderManager::Init() {
    TheShaderManager::Instance()->Load_all();
}

void TheRenderManager::Set_scene(Scene scene) {
    currentScene = scene;
}

void TheRenderManager::Set_render_size(int width, int height) {
    glViewport(0, 0, width, height);
    renderWidth = width;
    renderHeight = height;
}

void TheRenderManager::Voxelize(int size) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, size, size);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    TheShaderManager::Instance()->Use(Shaders::voxelize);
    TheShaderManager::Instance()->Set_uniform(Uniform::i1, "gridSize", &size);

    currentScene.Draw();

    glViewport(0, 0, renderWidth, renderHeight);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}

void TheRenderManager::Render(Camera* camera) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    TheShaderManager::Instance()->Use(Shaders::basic);
    TheShaderManager::Instance()->Set_uniform(Uniform::mat4,
            "view", glm::value_ptr(camera->view));
    TheShaderManager::Instance()->Set_uniform(Uniform::mat4,
            "projection", glm::value_ptr(camera->projection));

    currentScene.Draw();
}

