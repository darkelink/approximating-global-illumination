#pragma once

#include "camera.h"
#include "scene.h"

#include <GLFW/glfw3.h>

class Controller {
    public:
        Controller(GLFWwindow* window, Scene* scene);

        // controllers move cameras around
        void Set_camera(Camera* cam);

        // build what needs to be done based on input
        void Get_input();
        // perform actions
        void Update_view(float dt);

        float speed = 200.0f;
        bool shouldRender = true;

    private:
        Camera* camera;
        GLFWwindow* win;
        Scene* sce;

        // assume only 2d movement (x,z)
        glm::vec2 move;
        // assume 2d input for rotating (x,y in view space)
        glm::vec2 rot;

        double oldx, oldy;

        // movement speed
        // look sensitivity
        static constexpr float sens = 1.0f;
};
