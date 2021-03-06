#pragma once

#include "shader.h"

#include <GL/glew.h>
#include <GL/gl.h>

#include <glm/glm.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

class Camera {
    public:
        void Setup(float sceneSize);

        void Use();
        void Set_render_distance(float near, float far);
        void Set_aspect(float asp);

        // move and rotate in local space
        void Move(glm::vec3 direction);
        void Rotate(glm::vec3 direction);

        void Set_fov(float newFOV);

        // only need to update mvp once per frame
        void Update_view();

        // might as well set uniforms from here
        void Prepare_raytrace(int width, int height);

        glm::mat4 view;
        glm::mat4 mvp;

    private:
        glm::mat4 projection;

        glm::vec3 position;
        glm::vec3 rotation; // pitch, yaw, roll

        float fov;
        float aspect;

        float renderNear, renderFar;

        glm::vec3 get_camera_ray(float x, float y);
};
