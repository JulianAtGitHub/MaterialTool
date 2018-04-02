#ifndef PROGRAM_H
#define PROGRAM_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QVector>
#include <QList>
#include <QStringList>
#include <QSharedPointer>

#include "Interfaces.h"
#include "GLSystem.h"

namespace mtr {

struct ProgramDescription {
    QString vsFile;
    QString fsFile;
    QStringList macros;
    QList<QString> attribs;
    QMap<QString, TextureType> textures;
    QMap<QString, DataType> uniforms;
};

class Program : public QObject {
    Q_OBJECT

private:
    static QMap<DataUsage, QString> AttribUsage2Name;
    static QMap<UniformUsage, QString> UniformUsage2Name;
    static QMap<TextureUsage, QString> TextureUsage2Name;

    static QMap<QString, DataUsage> AttribName2Usage;
    static QMap<QString, UniformUsage> UniformName2Usage;
    static QMap<QString, TextureUsage> TextureName2Usage;

public:
    static void InitNameMappings(void);

public:
    explicit Program(ProgramDescription &description, QSharedPointer<GLSystem> &glSystem, QObject *parent = nullptr);
    virtual ~Program(void);

signals:

public slots:

public:
    inline bool Valid(void) { return _glProgram != 0; }
    inline const QList<TextureUsage> & SamplerUsages(void) const { return _samplers.keys(); }
    inline const QList<UniformUsage> & UniformUsages(void) const { return _uniforms.keys(); }

    bool Use(void);
    bool SetTexture(TextureUsage usage, uint texture, TextureType type);
    bool SetUniform(UniformUsage usage, void *value, DataType type);
    bool CheckAttribLocation(QList<int> &locations);

private:
    void Initialize(ProgramDescription &description);

    QWeakPointer<GLSystem> _glSystemRef;
    uint _glProgram;
    int _attribCount;
    QVector<bool> _attribs;
    QMap<TextureUsage, ShaderTexture> _samplers;
    QMap<UniformUsage, ShaderUniform> _uniforms;
};

}

#endif // PROGRAM_H
