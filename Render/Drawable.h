#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <QObject>
#include <QSharedPointer>
#include <QVector3D>
#include <QMatrix3x3>
#include <QMatrix4x4>
#include <QMap>

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
    inline const QVector3D & Position(void) const { return _position; }
    inline const QVector3D & Rotation(void) const { return _rotation; }
    inline const QVector3D & Scale(void) const { return _scale; }
    const QMatrix4x4 & ModelMatrix(void);
    const QMatrix3x3 & NormalMatrix(void);

    void Move(QVector3D &t);
    void Rotate(QVector3D &r);
    void Scale(QVector3D &s);

private:
    void LoadVertices(Model &model, QSharedPointer<GLSystem> &glSystem);
    void LoadTextures(Model &model, QSharedPointer<GLSystem> &glSystem);
    void CalculateMatrixes(void);

    QMap<TextureUsage, TextureInfo> _textures;
    QMap<UniformUsage, QByteArray> _values;
    VertexInfo _vertex;
    QVector3D _position;
    QVector3D _rotation;
    QVector3D _scale;
    QMatrix4x4 _modelMatrix;
    QMatrix3x3 _normalMatrix;
    bool _isDirty;
};

}

#endif // DRAWABLE_H
