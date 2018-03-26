#include <QString>
#include <QFile>
#include "stb_image.h"
#include "GLSystem.h"

namespace mtr {

const char * GLSystem::ShaderVersion = "#version 330 core";

GLSystem::GLSystem(void)
{
    initializeOpenGLFunctions();
}

GLSystem::~GLSystem(void) {

}

void GLSystem::GetSystemInfo(void) {
    _vendor = glGetString(GL_VENDOR);
    _renderer = glGetString(GL_RENDERER);
    _version = glGetString(GL_VERSION);
    _glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);

    QString extensions = glGetString(GL_EXTENSIONS);
    _extensions = extensions.split(" ", QString::SkipEmptyParts);

    qInfo << "vendor:       " << _vendor;
    qInfo << "renderer:     " << _renderer;
    qInfo << "version:      " << _version;
    qInfo << "glsl version: " << _glslVersion;
    qInfo << "extensions:   " << extensions;
}

bool GLSystem::LoadTexture(TextureInfo &textureInfo, const QString &imageFile, bool mipmaps) {
    if (imagePath.isEmpty()) {
        qCritical << "GLSystem::CreateTexture image path is empty!";
        return false;
    }

    int width, height, nComponent;
    std::string filePath = imageFile.toStdString();
    uchar *data = stbi_load(filePath.c_str(), &width, &height, &nComponent, 0);
    if (!data) {
        qCritical << "GLSystem::CreateTexture load image " << imageFile << " failed!";
        return false;
    }

    int level = 0;
    uint internalFormat = GL_RGB;
    uint border = 0;
    uint srcFormat = GL_RGB;
    uint srcType = GL_UNSIGNED_BYTE;

    switch (nComponent) {
        case 1: internalFormat = GL_RED; srcFormat = GL_RED; break;
        case 3: internalFormat = GL_RGB; srcFormat = GL_RGB; break;
        case 4: internalFormat = GL_RGBA; srcFormat = GL_RGBA; break;
        default:
            qCritical << "GLSystem::CreateTexture image format not support!";
            return false;
    }

    uint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, level, internalFormat, width, height, border, srcFormat, srcType, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    if (mipmaps) {
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER , GL_LINEAR);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER , GL_LINEAR);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);

    textureInfo.texture = texture;
    textureInfo.type = Texture2D;
    textureInfo.width = width;
    textureInfo.height = height;

    return true;
}

bool GLSystem::LoadHDRTexture(TextureInfo &texture, const QString &imageFile) {
    if (imagePath.isEmpty()) {
        qCritical << "GLSystem::LoadHDRTexture image path is empty!";
        return false;
    }

    int width, height, nComponent;
    std::string filePath = imageFile.toStdString();
    float *data = stbi_loadf(filePath.c_str(), &width, &height, &nComponent, 0);
    if (!data) {
        qCritical << "GLSystem::LoadHDRTexture load image " << imageFile << " failed!";
        return false;
    }

    int level = 0;
    uint internalFormat = GL_RGB16F;
    uint border = 0;
    uint srcFormat = GL_RGB;
    uint srcType = GL_FLOAT;

    uint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, level, internalFormat, width, height, border, srcFormat, srcType, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER , GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);

    textureInfo.texture = texture;
    textureInfo.type = Texture2D;
    textureInfo.width = width;
    textureInfo.height = height;

    return true;
}

bool GLSystem::ReadTextFile(QByteArray &content, const QString &filePath) {
    if (filePath.isEmpty()) {
        qCritical << "GLSystem::ReadTextFile file path is empty!";
        return false;
    }

    content.clear();

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical << "GLSystem::ReadTextFile can not open file:" << filePath;
        return false;
    }

    content = file.readAll();

    file.close();

    return true;
}

bool GLSystem::LoadShader(uint &shader, uint type, QByteArray &source, QByteArray &header) {
    if (source.isEmpty()) {
        qCritical << "GLSystem::LoadShader shader source is empty!";
        return false;
    }

    if (type != GL_VERTEX_SHADER && type != GL_FRAGMENT_SHADER) {
        qCritical << "GLSystem::LoadShader unknown shader type!";
        return false;
    }

    shader = glCreateShader(type);
    const char *data[2] = {header.constData(), source.constData()};
    glShaderSource(shader, 2, data, nullptr);
    glCompileShader(shader);

    int compileResult;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileResult);
    if (compileResult == GL_FALSE) {
        int logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

        int maxLength = logLength + 1;
        char *log = new char[maxLength];
        glGetShaderInfoLog(shader, maxLength, &logLength, log);

        qCritical << "GLSystem::LoadShader shader compile with error:" << log;
        delete [] log;

        glDeleteShader(shader);
        shader = 0;
        return false;
    }

    return true;
}

bool GLSystem::LoadProgram(uint &program, const QString &vsFile, const QString &fsFile, const QStringList &macros) {
    if (vsFile.isEmpty() || fsFile.isEmpty()) {
        qCritical << "GLSystem::LoadProgram invalid shader file path!";
        return false;
    }

    // read file content
    QByteArray vsSource;
    QByteArray fsSource;
    if (!ReadTextFile(vsSource, vsFile) || !ReadTextFile(fsSource, fsFile)) {
        qCritical << "GLSystem::LoadProgram failed when read shader file!";
        return false;
    }

    // assemble file header
    QByteArray header(ShaderVersion);
    header.push_back("\n");
    foreach (const QString macro, macros) {
        std::string str = macro.toStdString();
        header.push_back(str.c_str());
        header.push_back("\n");
    }

    uint vertexShader;
    uint fragmentShader;
    if (!LoadShader(vertexShader, GL_VERTEX_SHADER, vsSource, header)
        || !LoadShader(fragmentShader, GL_FRAGMENT_SHADER, fsSource, header)) {
        qCritical << "GLSystem::LoadProgram Load shader failed!";
        return false;
    }

    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    int linkResult;
    glGetProgramiv(program, GL_LINK_STATUS, &linkResult);
    if (linkResult == GL_FALSE) {
        int logLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

        int maxLength = logLength + 1;
        char *log = new char[maxLength];
        glGetProgramInfoLog(program, maxLength, &logLength, log);

        qCritical << "GLSystem::LoadProgram shader link with error:" << log;
        delete [] log;

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(program);
        program = 0;
        return false;
    }

    return true;
}
