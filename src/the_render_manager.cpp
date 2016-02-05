#include "the_render_manager.h"

#include "the_shader_manager.h"
#include "the_texture_manager.h"

#include <glm/gtc/type_ptr.hpp>

#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>

const GLfloat TheRenderManager::screenQuad[] = {
     1.0f, -1.0f,
     1.0f,  1.0f,
    -1.0f, -1.0f,
    -1.0f,  1.0f
};

TheRenderManager* TheRenderManager::instance = 0;

TheRenderManager* TheRenderManager::Instance() {
    if (!instance) {
        instance = new TheRenderManager;
    }
    return instance;
}

void TheRenderManager::Init(int width, int height) {
    TheShaderManager::Instance()->Load_all();
    renderWidth = width;
    renderHeight = height;
}

void TheRenderManager::Set_scene(Scene scene) {
    currentScene = scene;
}

void TheRenderManager::Set_render_size(int width, int height) {
    glViewport(0, 0, width, height);
    TheTextureManager::Instance()->Resize_framebuffer(width, height);
    renderWidth = width;
    renderHeight = height;
}

void TheRenderManager::Voxelize(int resolution) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, resolution, resolution);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    // need to count voxels to determine buffer size
    TheShaderManager::Instance()->Use(Shaders::voxelcount);
    TheShaderManager::Instance()->Set_uniform(Uniform::i1, "gridSize", &resolution);

    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, voxelCount);

    currentScene.Draw();


    TheShaderManager::Instance()->Use(Shaders::voxelize);
    TheShaderManager::Instance()->Set_uniform(Uniform::i1, "gridSize", &resolution);

    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, voxelCount);

    currentScene.Draw();


    glViewport(0, 0, renderWidth, renderHeight);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}

void TheRenderManager::Render(Camera* camera) {
    if (defered) {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        TheShaderManager::Instance()->Use(Shaders::defered);
    } else {
        TheShaderManager::Instance()->Use(Shaders::shaded);
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    TheShaderManager::Instance()->Set_uniform(Uniform::mat4,
            "view", glm::value_ptr(camera->view));
    TheShaderManager::Instance()->Set_uniform(Uniform::mat4,
            "mvp", glm::value_ptr(camera->mvp));

    currentScene.Draw();
    if (defered) {
        Render_framebuffer();
    }
}

void TheRenderManager::Render_framebuffer() {
    glDisable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    TheShaderManager::Instance()->Use(Shaders::screen);

    int i = 0;
    for (auto tex : TheTextureManager::Instance()->Get_framebuffer_textures()) {
        glUniformHandleui64ARB(i++, tex);
    }

    glBindVertexArray(quadVA);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    /*
     *glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
     *glReadBuffer(GL_COLOR_ATTACHMENT0);
     *glBlitFramebuffer(0, 0, renderWidth/2, renderHeight,
     *        0, 0, renderWidth/2, renderHeight,
     *        GL_COLOR_BUFFER_BIT, GL_LINEAR);
     *glReadBuffer(GL_COLOR_ATTACHMENT1);
     *glBlitFramebuffer(renderWidth/2, 0, renderWidth, renderHeight,
     *        renderWidth/2, 0, renderWidth, renderHeight,
     *        GL_COLOR_BUFFER_BIT, GL_LINEAR);
     */

    glEnable(GL_DEPTH_TEST);
}

void TheRenderManager::Use_defered() {
    if (!defered) {
        framebuffer = TheTextureManager::Instance()->
            Create_framebuffer(renderWidth, renderHeight);

        glGenVertexArrays(1, &quadVA);
        glBindVertexArray(quadVA);
        glGenBuffers(1, &quadbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, quadbuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(screenQuad), screenQuad,
                GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, quadbuffer);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    }

    defered = true;
}

void TheRenderManager::Init_voxelization() {
    const int zero = 0;

    glGenBuffers(1, &voxelCount);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, voxelCount);
    glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), &zero, GL_STATIC_DRAW);

    // count voxels

    int count = *(GLuint*)glMapBuffer(GL_ATOMIC_COUNTER_BUFFER, GL_MAP_READ_BIT);

    voxels.push_back(TheTextureManager::Instance()->Create_empty({count}, GL_R32UI));
    voxels.push_back(TheTextureManager::Instance()->Create_empty({count}, GL_RGBA8));
    voxels.push_back(TheTextureManager::Instance()->Create_empty({count}, GL_RGBA16F));
}
