#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <QObject>
#include <QSharedPointer>
#include <QMap>

#include "glm/vec3.hpp"
#include "glm/mat3x3.hpp"
#include "glm/mat4x4.hpp"

#include "Interfaces.h"
#include "GLSystem.h"

namespace mtr {

class Drawable : public QObject {
    Q_OBJECT

public:
    explicit Drawable(Model &model, QSharedPointer<GLSystem> &glSystem, QObject *parent = nullptr);
    virtual ~Drawable(void);

signals:

public slots:

public:
    inline const glm::vec3 & Position(void) const { return _position; }
    inline const glm::vec3 & Rotation(void) const { return _rotation; }
    inline const glm::vec3 & Scale(void) const { return _scale; }
    const glm::mat4 & ModelMatrix(void);
    const glm::mat3 & NormalMatrix(void);

    void Move(glm::vec3 &t);
    void Rotate(glm::vec3 &r);
    void Scale(glm::vec3 &s);

private:
    void LoadVertices(Model &model, QSharedPointer<GLSystem> &glSystem);
    void LoadTextures(Model &model, QSharedPointer<GLSystem> &glSystem);
    void CalculateMatrixes(void);

    QMap<TextureUsage, TextureInfo> _textures;
    QMap<UniformUsage, QByteArray> _values;
    VertexInfo _vertex;
    glm::vec3 _position;
    glm::vec3 _rotation;
    glm::vec3 _scale;
    glm::mat4 _modelMatrix;
    glm::mat3 _normalMatrix;
    bool _isDirty;
};

}

#endif // DRAWABLE_H
