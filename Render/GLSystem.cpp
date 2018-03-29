#include <QString>
#include <QFile>
#include "stb_image.h"
#include "GLSystem.h"

namespace mtr {

const char * GLSystem::ShaderVersion = "#version 330 core";

GLSystem::GLSystem(QObject *parent)
: QObject(parent)
{

}

GLSystem::~GLSystem(void) {

}

void GLSystem::GetSystemInfo(void) {
    _vendor = (const char *)glGetString(GL_VENDOR);
    _renderer = (const char *)glGetString(GL_RENDERER);
    _version = (const char *)glGetString(GL_VERSION);
    _glslVersion = (const char *)glGetString(GL_SHADING_LANGUAGE_VERSION);

    QString extensions = (const char *)glGetString(GL_EXTENSIONS);
    _extensions = extensions.split(" ", QString::SkipEmptyParts);

    qInfo("vendor:       %s", _vendor.toUtf8().data());
    qInfo("renderer:     %s", _renderer.toUtf8().data());
    qInfo("version:      %s", _version.toUtf8().data());
    qInfo("glsl version: %s", _glslVersion.toUtf8().data());
    qInfo("extensions:   %s", extensions.toUtf8().data());
}

void GLSystem::InitializeGL(void) {
    initializeOpenGLFunctions();

    GetSystemInfo();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
}

bool GLSystem::CreateTexture(TextureInfo &textureInfo, const QString &imageFile, bool mipmaps) {
    if (imageFile.isEmpty()) {
        qCritical("GLSystem::CreateTexture image path is empty!");
        return false;
    }

    int width, height, nComponent;
    QByteArray filePath = imageFile.toUtf8();
    uchar *data = stbi_load(filePath.data(), &width, &height, &nComponent, 0);
    if (!data) {
        qCritical("GLSystem::CreateTexture load image %s failed!", imageFile.toUtf8().data());
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
            qCritical("GLSystem::CreateTexture image format not support!");
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

bool GLSystem::CreateHDRTexture(TextureInfo &textureInfo, const QString &imageFile) {
    if (imageFile.isEmpty()) {
        qCritical("GLSystem::CreateHDRTexture image path is empty!");
        return false;
    }

    int width, height, nComponent;
    QByteArray filePath = imageFile.toUtf8();
    float *data = stbi_loadf(filePath.data(), &width, &height, &nComponent, 0);
    if (!data) {
        qCritical("GLSystem::CreateHDRTexture load image %s failed!", imageFile.toUtf8().data());
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
        qCritical("GLSystem::ReadTextFile file path is empty!");
        return false;
    }

    content.clear();

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical("GLSystem::ReadTextFile can not open file:%s", filePath.toUtf8().data());
        return false;
    }

    content = file.readAll();

    file.close();

    return true;
}

bool GLSystem::LoadShader(uint &shader, uint type, QByteArray &source, QByteArray &header) {
    if (source.isEmpty()) {
        qCritical("GLSystem::LoadShader shader source is empty!");
        return false;
    }

    if (type != GL_VERTEX_SHADER && type != GL_FRAGMENT_SHADER) {
        qCritical("GLSystem::LoadShader unknown shader type!");
        return false;
    }

    shader = glCreateShader(type);
    const char *data[2] = {header.data(), source.data()};
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

        qCritical("GLSystem::LoadShader shader compile with error:%s", log);
        delete [] log;

        glDeleteShader(shader);
        shader = 0;
        return false;
    }

    return true;
}

bool GLSystem::CreateProgram(uint &program, const QString &vsFile, const QString &fsFile, const QStringList &macros) {
    if (vsFile.isEmpty() || fsFile.isEmpty()) {
        qCritical("GLSystem::CreateProgram invalid shader file path!");
        return false;
    }

    // read file content
    QByteArray vsSource;
    QByteArray fsSource;
    if (!ReadTextFile(vsSource, vsFile) || !ReadTextFile(fsSource, fsFile)) {
        qCritical("GLSystem::CreateProgram failed when read shader file!");
        return false;
    }

    // assemble file header
    QByteArray header(ShaderVersion);
    header.push_back("\n");
    foreach (const QString &macro, macros) {
        header.push_back(macro.toUtf8());
        header.push_back("\n");
    }

    uint vertexShader;
    uint fragmentShader;
    if (!LoadShader(vertexShader, GL_VERTEX_SHADER, vsSource, header)
        || !LoadShader(fragmentShader, GL_FRAGMENT_SHADER, fsSource, header)) {
        qCritical("GLSystem::CreateProgram Load shader failed!");
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

        qCritical("GLSystem::CreateProgram shader link with error:%s", log);
        delete [] log;

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(program);
        program = 0;
        return false;
    }

    return true;
}

bool GLSystem::CreateBufferObject(uint &object, const void *data, int size, bool isElement) {
    if (data == nullptr || size == 0) {
        qCritical("GLSystem::CreateBufferObject buffer data is empty!");
        return false;
    }

    uint target = isElement ? GL_ELEMENT_ARRAY_BUFFER : GL_ARRAY_BUFFER;

    glGenBuffers(1, &object);
    glBindBuffer(target, object);
    glBufferData(target, size, data, GL_STATIC_DRAW);

    glBindBuffer(target, 0);
    return true;
}

bool GLSystem::CreateBufferObject(uint &object, QVector<float> &data) {
    return CreateBufferObject(object, data.data(), data.size() * sizeof(float), false);
}

bool GLSystem::CreateBufferObject(uint &object, QVector<short> &data, bool isElement) {
    return CreateBufferObject(object, data.data(), data.size() * sizeof(short), isElement);
}

bool GLSystem::CreateVertexObjects(VertexInfo &vertex, FloatData &vertices, ShortData &indices) {
    if (vertices.data.size() == 0) {
        qCritical("GLSystem::CreateVertexObjects vertex data is empty!");
        return false;
    }

    CreateBufferObject(vertex.vbo, vertices.data);
    if (indices.data.size() > 0) {
        CreateBufferObject(vertex.ebo, indices.data, true);
    } else {
        vertex.ebo = 0;
    }

    glGenVertexArrays(1, &vertex.vao);
    glBindVertexArray(vertex.vao);
    glBindBuffer(GL_ARRAY_BUFFER, vertex.vbo);

    int szFloat = sizeof(float);

    int stride = 0;
    for (int i = 0; i < vertices.layouts.count(); ++i) {
        const DataLayout &layout = vertices.layouts[i];
        switch(layout.type) {
            case Float: stride += szFloat; break;
            case Float2: stride += 2 * szFloat; break;
            case Float3: stride += 3 * szFloat; break;
            case Float4: stride += 4 * szFloat; break;
            default:
                qWarning("Invalod layout data type of vertices, type:%d", layout.type);
                break;
        }
    }

    size_t offset = 0;
    for (int i = 0; i < vertices.layouts.count(); ++i) {
        const DataLayout &layout = vertices.layouts[i];
        glEnableVertexAttribArray(layout.usage);

        int nComponent = 0;
        switch(layout.type) {
            case Float: nComponent = 1; break;
            case Float2: nComponent = 2; break;
            case Float3: nComponent = 3; break;
            case Float4: nComponent = 4; break;
            default:
                qWarning("Invalod layout data type of vertices, type:%d", layout.type);
                break;
        }
        glVertexAttribPointer(layout.usage, nComponent, GL_FLAT, GL_FALSE, stride, (const void *)offset);

        offset += nComponent * szFloat;
    }

    if (vertex.ebo != 0) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex.ebo);
    }

    glBindVertexArray(0);

    vertex.count = vertex.ebo != 0 ? indices.data.size() : vertices.data.size() / (stride / szFloat);
    vertex.layouts = vertices.layouts;

    return true;
}

bool GLSystem::CheckError(void) {
    uint error = glGetError();
    switch (error) {
      case GL_NO_ERROR: qInfo("NO_ERROR"); return true;
      case GL_INVALID_ENUM: qWarning("INVALID_ENUM"); break;
      case GL_INVALID_VALUE: qWarning("INVALID_VALUE"); break;
      case GL_INVALID_OPERATION: qWarning("INVALID_OPERATION"); break;
      case GL_INVALID_FRAMEBUFFER_OPERATION: qWarning("INVALID_FRAMEBUFFER_OPERATION"); break;
      case GL_OUT_OF_MEMORY: qWarning("OUT_OF_MEMORY"); break;
      default: qWarning("OpenGL Error:%d", error); break;
    }

    return false;
}

bool GLSystem::CheckFramebufferStatus(void) {
    uint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch(status) {
        case GL_FRAMEBUFFER_COMPLETE: qInfo("FRAMEBUFFER_COMPLETE"); return true;
        case GL_FRAMEBUFFER_UNDEFINED: qWarning("FRAMEBUFFER_UNDEFINED"); break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: qWarning("FRAMEBUFFER_INCOMPLETE_ATTACHMENT"); break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: qWarning("FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"); break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: qWarning("FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER"); break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: qWarning("FRAMEBUFFER_INCOMPLETE_READ_BUFFER"); break;
        case GL_FRAMEBUFFER_UNSUPPORTED: qWarning("FRAMEBUFFER_UNSUPPORTED"); break;
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: qWarning("FRAMEBUFFER_INCOMPLETE_MULTISAMPLE"); break;
        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: qWarning("FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS"); break;
        default: qWarning("Frame Buffer status error:%d", status); break;
    }

    return false;
}

}
