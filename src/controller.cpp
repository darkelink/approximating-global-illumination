#include "controller.h"
#include "the_shader_manager.h"
#include "the_render_manager.h"

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>

constexpr float Controller::sens;

Controller::Controller(GLFWwindow* window, Scene* scene) {
    win = window;
    sce = scene;
}

void Controller::Set_camera(Camera* cam) {
    camera = cam;
}

void Controller::Get_input() {
    // misc functions
    if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(win, GL_TRUE);
    }

    // ensure reloading only happens once per key press
    static bool isPressed = false;
    if (glfwGetKey(win, GLFW_KEY_ENTER) == GLFW_PRESS && !isPressed) {
        std::cout << "Reloading shaders... ";
        TheShaderManager::Instance()->Load_all();
        std::cout << "done" << std::endl;
        isPressed = true;
    }
    if (isPressed && glfwGetKey(win, GLFW_KEY_ENTER) == GLFW_RELEASE) {
        isPressed = false;
    }

    // set render mode
    if (glfwGetKey(win, GLFW_KEY_1) == GLFW_PRESS) {
        TheRenderManager::Instance()->currentRenderer = RenderType::phong;
    }
    if (glfwGetKey(win, GLFW_KEY_2) == GLFW_PRESS) {
        TheRenderManager::Instance()->currentRenderer = RenderType::voxelPoints;
    }
    if (glfwGetKey(win, GLFW_KEY_3) == GLFW_PRESS) {
        TheRenderManager::Instance()->currentRenderer = RenderType::voxelCubes;
    }

    // movement
    //
    move = glm::vec2(0,0);

    if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS) {
        move += glm::vec2(1,0);
    }
    if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS) {
        move -= glm::vec2(1,0);
    }
    if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS) {
        move += glm::vec2(0,1);
    }
    if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS) {
        move -= glm::vec2(0,1);
    }

    glm::normalize(move);

    if (glfwGetKey(win, GLFW_KEY_SPACE) == GLFW_PRESS) {
        move *= 5;
    }

    // rotation
    //
    double xpos, ypos;
    glfwGetCursorPos(win, &xpos, &ypos);

    rot = glm::vec2(xpos-oldx, ypos-oldy);

    oldx = xpos;
    oldy = ypos;
}

void Controller::Update_view(float dt) {
    // convert 2d deltas to pitch,yaw,roll
    camera->Rotate(glm::vec3(rot.y, rot.x, 0) * sens * dt);

    camera->Move(glm::vec3(move.x, 0, move.y) * speed * dt);

    camera->Update_view();
}

