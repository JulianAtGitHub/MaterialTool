#ifndef CAMERA_H
#define CAMERA_H

#include <QObject>

#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"

namespace mtr {

class Camera : public QObject {
    Q_OBJECT

public:
    explicit Camera(float fov, float aspect, QObject *parent = nullptr);
    virtual ~Camera(void);

signals:

public slots:

public:
    inline const glm::vec3 & Eye(void) const { return _eye; }
    inline const glm::vec3 & LookAt(void) const { return _lookAt; }
    inline const glm::vec3 & Up(void) const { return _up; }
    inline float FOV(void) const { return _fov; }
    inline float Aspect(void) const { return _aspect; }
    inline float Near(void) const { return _near; }
    inline float Far(void) const { return _far; }

    const glm::mat4 & ViewMatrix(void);
    const glm::mat4 & ProjMatrix(void);
    const glm::mat4 & ViewPorjMatrix(void);

private:
    void CalculateMatrixes(void);

    glm::vec3 _eye;
    glm::vec3 _lookAt;
    glm::vec3 _up;
    float _fov;     // field of view
    float _aspect;  // ratio of width divided by height
    float _near;
    float _far;
    glm::vec3 _rotation;
    glm::mat4 _viewMatrix;
    glm::mat4 _projMatrix;
    glm::mat4 _viewProjMatrix;
    bool _isDirty;
};

}

#endif // CAMERA_H
