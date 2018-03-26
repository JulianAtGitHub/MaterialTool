#ifndef MTR_GLSYSTEM_H
#define MTR_GLSYSTEM_H

#include <QtGlobal>
#include <QStringList>
#include <QByteArray>
#include <QOpenGLFunctions_3_3_Core>

#include "Interfaces.h"

namespace mtr {

class GLSystem : public QOpenGLFunctions_3_3_Core {
    static const char *ShaderVersion;

public:
    GLSystem(void);
    virtual ~GLSystem(void);

    bool LoadTexture(TextureInfo &texture, const QString &imageFile, bool mipmaps = true);
    // mipmaps is disabled when load  from HDR image
    bool LoadHDRTexture(TextureInfo &texture, const QString &imageFile);

    bool LoadProgram(uint &program, const QString &vsFile, const QString &fsFile, const QStringList &macros);

private:
    void GetSystemInfo(void);
    bool ReadTextFile(QByteArray &content, const QString &filePath);
    // type:  GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
    bool LoadShader(uint &shader, uint type, QByteArray &source, QByteArray &header);

private:
    QString _vendor;
    QString _renderer;
    QString _version;
    QString _glslVersion;
    QStringList _extensions;
};

}

#endif // MTR_GLSYSTEM_H
