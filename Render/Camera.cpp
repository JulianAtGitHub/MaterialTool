#include "Camera.h"
#include "glm/gtc/matrix_transform.hpp"

namespace mtr {

Camera::Camera(float fov, float aspect, QObject *parent)
: QObject(parent)
, _eye(0.0f, 0.0f, 0.0f)
, _lookAt(0.0f, 0.0f, -1.0f)
, _up(0.0f, 1.0f, 0.0f)
, _fov(fov)
, _aspect(aspect)
, _near(0.1f)
, _far(100.0f)
, _rotation(0.0f, 0.0f, 0.0f)
, _isDirty(true)
{

}

Camera::~Camera(void) {

}

void Camera::CalculateMatrixes(void) {
    glm::mat4 rotateMatrix;

    rotateMatrix = glm::rotate(glm::mat4(), _rotation.x, glm::vec3(0.0f, 1.0f, 0.0f));
    rotateMatrix = glm::rotate(rotateMatrix, _rotation.y, glm::vec3(1.0f, 0.0f, 0.0f));
    glm::vec3 lookAt = glm::vec3(rotateMatrix * glm::vec4(_lookAt, 1.0f));
    glm::vec3 up = glm::vec3(rotateMatrix * glm::vec4(_up, 1.0f));

    _viewMatrix = glm::lookAtRH(_eye, lookAt, up);
    _projMatrix = glm::perspectiveRH(_fov, _aspect, _near, _far);

    _viewProjMatrix = _projMatrix * _viewMatrix;
}

const glm::mat4 & Camera::ViewMatrix(void) {
    if (_isDirty) {
        CalculateMatrixes();
        _isDirty = false;
    }
    return _viewMatrix;
}

const glm::mat4 & Camera::ProjMatrix(void) {
    if (_isDirty) {
        CalculateMatrixes();
        _isDirty = false;
    }
    return _projMatrix;
}

const glm::mat4 & Camera::ViewPorjMatrix(void) {
    if (_isDirty) {
        CalculateMatrixes();
        _isDirty = false;
    }
    return _viewProjMatrix;
}


}
