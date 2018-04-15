#include "Camera.h"
#include "glm/gtc/matrix_transform.hpp"

namespace mtp {

Camera::Camera(float fov, float aspect)
: _eye(0.0f, 0.0f, 0.0f)
, _lookAt(0.0f, 0.0f, -1.0f)
, _up(0.0f, 1.0f, 0.0f)
, _fov(fov)
, _aspect(aspect)
, _near(0.1f)
, _far(100.0f)
, _rotation(0.0f, 0.0f, 0.0f)
, _mouseDown(false)
, _xRotation(0)
, _yRotation(0)
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

void Camera::OnMouseDown(float x, float y) {
    _mouseDown = true;
}

void Camera::OnMouseUp(void) {
    _mouseDown = false;
}

void Camera::OnMouseDrag(float deltaX, float deltaY) {
    if (!_mouseDown) { return; }
  
    _xRotation -= deltaX * 0.125;
    if (_xRotation < -45.0) { _xRotation = -45.0; }
    if (_xRotation > 45.0) { _xRotation = 45.0; }
  
    _yRotation -= deltaY * 0.125;
    if (_yRotation < -45.0) { _yRotation = -45.0; }
    if (_yRotation > 45.0) { _yRotation = 45.0; }

    UpdateRotation();
}

void Camera::UpdateRotation(void) {
    float x = _xRotation * (M_PI / 180.0);
    float y = _yRotation * (M_PI / 180.0);

    _rotation.x = x;
    if (_rotation.x > _fov) {_rotation.x = _fov;}
    if (_rotation.x < -_fov) {_rotation.x = -_fov;}

    _rotation.y = y;
    if (_rotation.y > _fov) {_rotation.y = _fov;}
    if (_rotation.y < -_fov) {_rotation.y = -_fov;}

    _isDirty = true;
}

}
