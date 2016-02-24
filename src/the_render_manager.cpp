#include "the_render_manager.h"

#include "the_shader_manager.h"
#include "the_texture_manager.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>


// only really need this here and doesn't fit in the class
// shamelessly taken from http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
int Round_next_power2(unsigned int v) {
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;

    return v;
}

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
    currentRenderer = RenderType::phong;

    // create a full screen quad
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

void TheRenderManager::Set_scene(Scene scene) {
    currentScene = scene;
}

void TheRenderManager::Set_render_size(int width, int height) {
    glViewport(0, 0, width, height);
    if (defered) {
        TheTextureManager::Instance()->Resize_framebuffer(width, height);
    }
    if (raytrace) {
        // cba resizing, delete and remake
        TheTextureManager::Instance()->Delete(raytraceTex);
        raytraceTex = TheTextureManager::Instance()->Create_empty(
                {renderWidth, renderHeight}, GL_RGBA);
    }
    renderWidth = width;
    renderHeight = height;
}

void TheRenderManager::Voxelize() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // camera should be same size as voxel grid
    glViewport(0, 0, voxelResolution, voxelResolution);
    // need to render everything
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    // don't write to framebuffer
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    TheShaderManager::Instance()->Use(Shaders::voxelize);
    TheShaderManager::Instance()->Set_uniform(Uniform::i1,
            "gridSize", &voxelResolution);
    TheShaderManager::Instance()->Set_uniform(Uniform::i1,
            "gridSize2", &voxelResolution);

    TheShaderManager::Instance()->Set_uniform(Uniform::mat4,
            "xproj", &xorth);
    TheShaderManager::Instance()->Set_uniform(Uniform::mat4,
            "yproj", &yorth);
    TheShaderManager::Instance()->Set_uniform(Uniform::mat4,
            "zproj", &zorth);

    // TODO: make bindless
    glBindImageTexture(0, voxels, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32UI);

    currentScene.Draw();

    // image writes are not coherent
    // make sure they are all done before continuing
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    // revert gl state
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

    // need to translate world space to voxel space
    float scale = currentScene.size*2/voxelResolution;

    switch (currentRenderer) {
        case RenderType::phong:
            TheShaderManager::Instance()->Use(Shaders::shaded);
            TheShaderManager::Instance()->Set_uniform(Uniform::mat4,
                    "view", glm::value_ptr(camera->view));
            TheShaderManager::Instance()->Set_uniform(Uniform::mat4,
                    "mvp", glm::value_ptr(camera->mvp));
            currentScene.Draw();
            break;
        case RenderType::voxelPoints:
            Voxelize();
            TheShaderManager::Instance()->Use(Shaders::drawPoints);
            TheShaderManager::Instance()->Set_uniform(Uniform::i1,
                    "resolution", &voxelResolution);
            TheShaderManager::Instance()->Set_uniform(Uniform::mat4,
                    "mvp", glm::value_ptr(camera->mvp));
            TheShaderManager::Instance()->Set_uniform(Uniform::f1,
                    "scale", &scale);
            glBindImageTexture(0, voxels, 0, GL_TRUE, 0,
                    GL_READ_ONLY, GL_R32UI);
            // draw a point at every location and discard in the shader
            glDrawArrays(GL_POINTS, 0,
                    voxelResolution * voxelResolution * voxelResolution);
            break;
        case RenderType::raytrace:
            Raytrace(camera);
            break;
    }

    if (defered) {
        Render_framebuffer();
    }
}

void TheRenderManager::Render_framebuffer() {
    if (defered) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        TheShaderManager::Instance()->Use(Shaders::composite);

        int i = 0;
        for (auto tex : TheTextureManager::Instance()->
                Get_framebuffer_textures()) {
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
    }
}

void TheRenderManager::Raytrace(Camera* camera) {
    TheShaderManager::Instance()->Use(Shaders::raytrace);


    // TODO: make bindless
    glBindImageTexture(0, voxels, 0, GL_TRUE, 0, GL_READ_ONLY, GL_R32UI);
    //glUniformHandleui64ARB(0, raytraceTex);

    glBindImageTexture(0,
            TheTextureManager::Instance()->GetID(raytraceTex),
            0, false, 0, GL_WRITE_ONLY, GL_RGBA8);


    float scale = currentScene.size*2/voxelResolution;
    TheShaderManager::Instance()->Set_uniform(Uniform::f1,
            "scale", &scale);
    TheShaderManager::Instance()->Set_uniform(Uniform::i1,
            "voxelResolution", &voxelResolution);

    // camera parameters are handled by the camera this time
    camera->Prepare_raytrace(renderWidth, renderHeight);
    //camera->Print_debug(scale);


    int workSize[3];
    TheShaderManager::Instance()->Get_info(GL_COMPUTE_WORK_GROUP_SIZE,
            workSize);

    // commit
    glDispatchCompute(
            Round_next_power2(renderWidth)/workSize[0],
            Round_next_power2(renderHeight)/workSize[1],
            1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    // render result texture to full screen quad
    TheShaderManager::Instance()->Use(Shaders::screenquad);
    glUniformHandleui64ARB(0, raytraceTex);

    glBindVertexArray(quadVA);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}


void TheRenderManager::Use_defered() {
    if (!defered) {
        framebuffer = TheTextureManager::Instance()->
            Create_framebuffer(renderWidth, renderHeight);
    }

    defered = true;
}

void TheRenderManager::Init_voxelization(int resolution) {
    voxelResolution = resolution;
    voxels = TheTextureManager::Instance()->Create_voxel_store(resolution);

    {
        using namespace glm;
        mat4 ortho = glm::ortho(-currentScene.size, currentScene.size,
                -currentScene.size, currentScene.size,
                -currentScene.size, currentScene.size);
        xorth = ortho * lookAt(glm::vec3(0,0,0), vec3(1,0,0), vec3(0,1,0));
        yorth = ortho * lookAt(glm::vec3(0,0,0), vec3(0,1,0), vec3(1,0,0));
        zorth = ortho * lookAt(glm::vec3(0,0,0), vec3(0,0,1), vec3(0,1,0));
    }
}

void TheRenderManager::Init_raytrace() {
    if (!raytrace) {
        raytraceTex = TheTextureManager::Instance()->Create_empty(
                {renderWidth, renderHeight}, GL_RGBA);
    }
    raytrace = true;
}
