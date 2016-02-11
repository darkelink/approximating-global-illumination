#include "the_render_manager.h"

#include "the_shader_manager.h"
#include "the_texture_manager.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
    if (defered) {
        TheTextureManager::Instance()->Resize_framebuffer(width, height);
    }
    renderWidth = width;
    renderHeight = height;
}

void TheRenderManager::Voxelize() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, voxelResolution, voxelResolution);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    TheShaderManager::Instance()->Use(Shaders::voxelize);
    TheShaderManager::Instance()->Set_uniform(Uniform::i1,
            "gridSize", &voxelResolution);

    TheShaderManager::Instance()->Set_uniform(Uniform::mat4,
            "xproj", &xorth);
    TheShaderManager::Instance()->Set_uniform(Uniform::mat4,
            "yproj", &yorth);
    TheShaderManager::Instance()->Set_uniform(Uniform::mat4,
            "zproj", &zorth);

    glBindImageTexture(0, voxels, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32UI);

    currentScene.Draw();

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);


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

void TheRenderManager::Init_voxelization(int resolution) {
    voxelResolution = resolution;
    voxels = TheTextureManager::Instance()->Create_voxel_store(resolution);

    glm::mat4 ortho = glm::ortho(-currentScene.size, currentScene.size,
            -currentScene.size, currentScene.size,
            -currentScene.size, currentScene.size);
    xorth = ortho * glm::lookAt(glm::vec3(1,0,0), glm::vec3(0,0,0), glm::vec3(0,1,0));
    yorth = ortho * glm::lookAt(glm::vec3(0,1,0), glm::vec3(0,0,0), glm::vec3(1,0,0));
    zorth = ortho * glm::lookAt(glm::vec3(0,0,1), glm::vec3(0,0,0), glm::vec3(0,1,0));
}
