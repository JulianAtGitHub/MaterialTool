#ifndef PROGRAM_H
#define PROGRAM_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QVector>
#include <QList>

#include "Interfaces.h"

namespace mtr {

class Program : public QObject {
    static QMap<DataUsage, QString> AttribUsage2Name;
    static QMap<UniformUsage, QString> UniformUsage2Name;
    Q_OBJECT

public:
    explicit Program(QObject *parent = nullptr);
    virtual ~Program(void);

signals:

public slots:

public:

private:
    uint _glProgram;
    uint _attribCount;
    QVector<int> _attribLocations;
    QList<ShaderTexture> _samplers;
    QMap<UniformUsage, ShaderUniform> _uniforms;
};

}

#endif // PROGRAM_H
