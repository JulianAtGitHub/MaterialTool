#include "Drawable.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"

namespace mtr {

Drawable::Drawable(Model &model, QSharedPointer<GLSystem> &glSystem, QObject *parent)
: QObject(parent)
, _isDirty(true)
, _position(0.0f, 0.0f, 0.0f)
, _rotation(0.0f, 0.0f, 0.0f)
, _scale(1.0f, 1.0f, 1.0f)
{
    LoadVertices(model, glSystem);
    LoadTextures(model, glSystem);
    _values = model.values;
}

Drawable::~Drawable(void) {

}

void Drawable::LoadVertices(Model &model, QSharedPointer<GLSystem> &glSystem) {
    _vertex.mode = model.mesh.mode;
    glSystem->CreateVertexObjects(_vertex, model.mesh.vertices, model.mesh.indices);
}

void Drawable::LoadTextures(Model &model, QSharedPointer<GLSystem> &glSystem) {
    QMapIterator<TextureUsage, QString> iter(model.textures);
    while(iter.hasNext()) {
        iter.next();
        TextureUsage usage = iter.key();
        const QString &name = iter.value();
        TextureInfo texture;
        if (glSystem->CreateTexture(texture, name)) {
            _textures[usage] = texture;
        } else {
            QByteArray cname = name.toUtf8();
            qWarning("Drawable::LoadTextures load texture %s failed!", cname.data());
        }
    }
}

void Drawable::CalculateMatrixes(void) {
    _modelMatrix = glm::scale(glm::mat4(), _scale);
    _modelMatrix = glm::rotate(_modelMatrix, _rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    _modelMatrix = glm::rotate(_modelMatrix, _rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    _modelMatrix = glm::rotate(_modelMatrix, _rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    _modelMatrix = glm::translate(_modelMatrix, _position);

    _normalMatrix = glm::mat3(glm::inverseTranspose(_modelMatrix));
}

const glm::mat4 & Drawable::ModelMatrix(void) {
    if (_isDirty) {
        CalculateMatrixes();
        this->_isDirty = false;
    }
    return _modelMatrix;
}

const glm::mat3 & Drawable::NormalMatrix(void) {
    if (_isDirty) {
        CalculateMatrixes();
        this->_isDirty = false;
    }
    return _normalMatrix;
}

void Drawable::Move(glm::vec3 &t) {
    _position += t;
    this->_isDirty = true;
}

void Drawable::Rotate(glm::vec3 &r) {
    _rotation += r;
    this->_isDirty = true;
}

void Drawable::Scale(glm::vec3 &s) {
    _scale += s;
    this->_isDirty = true;
}

}
