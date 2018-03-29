#include "Drawable.h"

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
    _modelMatrix.setToIdentity();
    _modelMatrix.scale(_scale);
    _modelMatrix.rotate(_rotation.x(), QVector3D(1.0f, 0.0f, 0.0f));
    _modelMatrix.rotate(_rotation.y(), QVector3D(0.0f, 1.0f, 0.0f));
    _modelMatrix.rotate(_rotation.z(), QVector3D(0.0f, 0.0f, 1.0f));
    _modelMatrix.translate(_position);

    _normalMatrix = _modelMatrix.normalMatrix();
}

const QMatrix4x4 & Drawable::ModelMatrix(void) {
    if (_isDirty) {
        CalculateMatrixes();
        this->_isDirty = false;
    }
    return _modelMatrix;
}

const QMatrix3x3 & Drawable::NormalMatrix(void) {
    if (_isDirty) {
        CalculateMatrixes();
        this->_isDirty = false;
    }
    return _normalMatrix;
}

void Drawable::Move(QVector3D &t) {
    _position += t;
    this->_isDirty = true;
}

void Drawable::Rotate(QVector3D &r) {
    _rotation += r;
    this->_isDirty = true;
}

void Drawable::Scale(QVector3D &s) {
    _scale += s;
    this->_isDirty = true;
}

}
