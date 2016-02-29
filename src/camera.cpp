#define GLM_SWIZZLE
#include "camera.h"

#include "the_shader_manager.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>

void Camera::Setup(float sceneSize) {
    // load some default values
    position = glm::vec3(0,0,0);
    rotation = glm::vec3(0,0,0);
    renderNear = sceneSize/100;
    renderFar = sceneSize*2;
    aspect = 16.0f/9.0f;
    fov = 90.0f;

    projection = glm::perspective(glm::radians(fov), aspect, renderNear, renderFar);
}

void Camera::Set_render_distance(float near, float far) {
    renderNear = near;
    renderFar = far;

    projection = glm::perspective(glm::radians(fov), aspect, renderNear, renderFar);
}

void Camera::Set_aspect(float asp) {
    aspect = asp;

    projection = glm::perspective(glm::radians(fov), aspect, renderNear, renderFar);
}

void Camera::Move(glm::vec3 direction) {
    // convert local space to world space
    // ignore technical vertical movement for now
    position.x -= sin(rotation.y) * direction.x + cos(rotation.y) * direction.z;
    position.y += sin(rotation.x) * direction.x;
    position.z += cos(rotation.y) * direction.x - sin(rotation.y) * direction.z;
}

void Camera::Rotate(glm::vec3 direction) {
    static constexpr float halfPi = 1.57079632679;
    rotation += direction;
    rotation.x = glm::clamp(rotation.x, -halfPi, halfPi);
}

void Camera::Set_fov(float newFOV) {
    fov = newFOV;
    projection = glm::perspective(glm::radians(fov), aspect, renderNear, renderFar);
}

void Camera::Update_view() {
    view = glm::mat4();

    // probably a faster way to do this
    view = glm::rotate(view, rotation.x, glm::vec3(1,0,0));
    view = glm::rotate(view, rotation.y, glm::vec3(0,1,0));
    //view = glm::rotate(view, rotation.z, glm::vec3(0,0,1));
    view = glm::translate(view, position);

    mvp = projection * view;
}

void Camera::Prepare_raytrace(int width, int height) {
    TheShaderManager::Instance()->Set_uniform(Uniform::vec3,
            "camera", &position);

    glm::vec3 asdf;

    asdf = get_camera_ray(-1,-1);
    TheShaderManager::Instance()->Set_uniform(Uniform::vec3,
            "bottomleft", &asdf);

    asdf = get_camera_ray(-1,1);
    TheShaderManager::Instance()->Set_uniform(Uniform::vec3,
            "topleft", &asdf);

    asdf = get_camera_ray(1,-1);
    TheShaderManager::Instance()->Set_uniform(Uniform::vec3,
            "bottomright", &asdf);

    asdf = get_camera_ray(1,1);
    TheShaderManager::Instance()->Set_uniform(Uniform::vec3,
            "topright", &asdf);
}

void Camera::Print_debug(float scale) {
    std::cout << "position: " << glm::to_string(position) << std::endl;

    glm::vec3 asdf;

    std::cout << "direction: " << glm::to_string(get_camera_ray(0,0)) << std::endl;

    std::cout << "topleft: " << glm::to_string(get_camera_ray(-1,1)) << std::endl;
    std::cout << "topright: " << glm::to_string(get_camera_ray(1,1)) << std::endl;
    std::cout << "bottomleft: " << glm::to_string(get_camera_ray(-1,-1)) << std::endl;
    std::cout << "bottomright: " << glm::to_string(get_camera_ray(1,-1)) << std::endl;
}

glm::vec3 Camera::get_camera_ray(float x, float y) {
    glm::vec4 working = glm::inverse(projection) * glm::vec4(x,y,0,1);
    working.w = 0;
    working = glm::inverse(view) * working;
    return glm::normalize(working.xyz());
}
