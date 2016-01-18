#include "scene.h"
#include "the_shader_manager.h"
#include "the_render_manager.h"
#include "camera.h"
#include "controller.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>

int
main(int argc, char* argv[]) {
    std::cout << "Loading GLFW... ";
    if (!glfwInit()) {
        std::cerr << "ERROR: could not start GLFW" << std::endl;
        return 1;
    }
    std::cout << "done" << std::endl;

    int winWidth = 1280;
    int winHeight = 720;

    int testHeight, testWidth;

    GLFWwindow* window = glfwCreateWindow(winWidth, winHeight, "Voxels", NULL, NULL);
    if (!window) {
        std::cerr << "ERROR: could not open window" << std::endl;
        glfwTerminate();
        return 2;
    }
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    glfwSwapInterval(0);

    std::cout << "Loading GLEW... ";
    if (glewInit() != GLEW_OK) {
        std::cerr << "ERROR: could not start GLEW" << std::endl;
        return 3;
    }
    std::cout << "done" << std::endl;

    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version  = glGetString(GL_VERSION);
    std::cout << "Renderer: " << renderer << std::endl;
    std::cout << "OpenGL version: " << version << std::endl;

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPos(window, 0, 0);

    glfwPollEvents();


    std::cout << "Loading shaders... ";
    TheShaderManager::Instance()->Load_all();
    TheShaderManager::Instance()->Use(Shaders::basic);
    std::cout << "done" << std::endl;

    std::cout << "Loading scene... ";
    Scene scene;
    scene.Load_obj_file(argv[1]);
    std::cout << "done" << std::endl;


    Camera camera;
    camera.Setup();

    Controller controller(window, &scene);
    controller.Set_camera(&camera);

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    double currentTime, lastTime = 0;
    float deltaTime;


    std::cout << "Entering main loop" << std::endl;
    do {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        currentTime = glfwGetTime();
        deltaTime = float(currentTime - lastTime);
        lastTime = currentTime;

        controller.Get_input();
        controller.Update_view(deltaTime);

        // should this be done somewhere else?
        TheShaderManager::Instance()->Set_uniform(Uniform::mat4,
                "MVP", glm::value_ptr(camera.mvp));

        scene.Draw();

        glfwSwapBuffers(window);
        glfwPollEvents();

        // resize window
        glfwGetWindowSize(window, &testWidth, &testHeight);
        if (testWidth != winWidth || testHeight != winHeight) {
            winWidth = testWidth;
            winHeight = testHeight;
            TheRenderManager::Instance()->Set_render_size(winWidth, winHeight);
            camera.Set_aspect((float)winWidth/winHeight);
        }

    } while(!glfwWindowShouldClose(window));

    std::cout << "Window closed, shutting down" << std::endl;

    glfwTerminate();

    return 0;
}
