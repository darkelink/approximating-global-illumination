#include "the_render_manager.h"

#include "the_shader_manager.h"

#include <GL/glew.h>
#include <GL/gl.h>

TheRenderManager* TheRenderManager::instance = 0;

TheRenderManager* TheRenderManager::Instance() {
    if (!instance) {
        instance = new TheRenderManager;
    }
    return instance;
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

void TheRenderManager::Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    currentScene.Draw();
}

