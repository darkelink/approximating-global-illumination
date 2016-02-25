#define GLM_SWIZZLE
#include "camera.h"

#include "the_shader_manager.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>

void Camera::Setup() {
    // load some default values
    position = glm::vec3(0,0,0);
    rotation = glm::vec3(0,0,0);
    renderNear = 10.0f;
    renderFar = 5000.0f;
    aspect = 1.0f;
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
    view = glm::rotate(view, rotation.z, glm::vec3(0,0,1));
    view = glm::translate(view, position);

    mvp = projection * view;
}

void Camera::Prepare_raytrace(int width, int height) {
    TheShaderManager::Instance()->Set_uniform(Uniform::vec3,
            "camera", &position);

    glm::vec3 asdf = glm::vec3(0,0,1);
    asdf = glm::rotate(asdf, rotation.x, glm::vec3(1,0,0));
    asdf = glm::rotate(asdf, rotation.y, glm::vec3(0,1,0));
    asdf = glm::rotate(asdf, rotation.z, glm::vec3(0,0,1));

    float vfov = glm::radians(fov);
    float hfov = 2*glm::atan(glm::tan(vfov/2)*aspect);

    asdf = glm::rotate(asdf, -vfov/2, glm::vec3(1,0,0));
    asdf = glm::rotate(asdf, -hfov/2, glm::vec3(0,1,0));
    TheShaderManager::Instance()->Set_uniform(Uniform::vec3,
            "topleft", &asdf);

    asdf = glm::rotate(asdf, hfov, glm::vec3(0,1,0));
    TheShaderManager::Instance()->Set_uniform(Uniform::vec3,
            "topright", &asdf);

    asdf = glm::vec3(0,0,1);
    asdf = glm::rotate(asdf, rotation.x, glm::vec3(1,0,0));
    asdf = glm::rotate(asdf, rotation.y, glm::vec3(0,1,0));
    asdf = glm::rotate(asdf, rotation.z, glm::vec3(0,0,1));

    asdf = glm::rotate(asdf, vfov/2, glm::vec3(1,0,0));
    asdf = glm::rotate(asdf, hfov/2, glm::vec3(0,1,0));
    TheShaderManager::Instance()->Set_uniform(Uniform::vec3,
            "bottomright", &asdf);

    asdf = glm::rotate(asdf, -hfov, glm::vec3(0,1,0));
    TheShaderManager::Instance()->Set_uniform(Uniform::vec3,
            "bottomleft", &asdf);
}

void Camera::Print_debug(float scale) {
    //std::cout << "camera: " << glm::to_string(position) << std::endl;

    glm::vec3 asdf = glm::vec3(0,0,1);

    asdf = glm::rotate(asdf, rotation.x, glm::vec3(1,0,0));
    asdf = glm::rotate(asdf, rotation.y, glm::vec3(0,1,0));

    float vfov = glm::radians(fov);
    float hfov = 2*glm::atan(glm::tan(vfov/2)*aspect);

    std::cout << "direction: " << glm::to_string(asdf) << std::endl;

    asdf = glm::rotate(asdf, -vfov/2, glm::vec3(1,0,0));
    asdf = glm::rotate(asdf, -hfov/2, glm::vec3(0,1,0));
    std::cout << "topleft: " << glm::to_string(asdf) << std::endl;

    asdf = glm::rotate(asdf, hfov, glm::vec3(0,1,0));
    std::cout << "topright: " << glm::to_string(asdf) << std::endl;

    asdf = glm::rotate(asdf, -vfov, glm::vec3(1,0,0));
    std::cout << "bottomright: " << glm::to_string(asdf) << std::endl;

    asdf = glm::rotate(asdf, -hfov, glm::vec3(0,1,0));
    std::cout << "bottomleft: " << glm::to_string(asdf) << std::endl;
}
