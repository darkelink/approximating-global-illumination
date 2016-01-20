#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

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
    mvp = projection;

    // probably a faster way to do this
    mvp = glm::rotate(mvp, rotation.x, glm::vec3(1,0,0));
    mvp = glm::rotate(mvp, rotation.y, glm::vec3(0,1,0));
    mvp = glm::rotate(mvp, rotation.z, glm::vec3(0,0,1));
    mvp = glm::translate(mvp, position);

    // forget about model, 2/3 ain't bad
}
