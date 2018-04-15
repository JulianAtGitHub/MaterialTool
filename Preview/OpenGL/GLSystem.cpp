#include <fstream>
#include <sstream>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "GLSystem.h"
#include "Utilities.h"

namespace mtp {

const char * GLSystem::ShaderVersion = "#version 330 core \n";

GLSystem::GLSystem(void)
{

}

GLSystem::~GLSystem(void) {

}

void GLSystem::GetSystemInfo(void) {
    _vendor = (const char *)glGetString(GL_VENDOR);
    _renderer = (const char *)glGetString(GL_RENDERER);
    _version = (const char *)glGetString(GL_VERSION);
    _glslVersion = (const char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
    
    cout << "vendor:       " << _vendor.c_str() << endl;
    cout << "renderer:     " << _renderer.c_str() << endl;
    cout << "version:      " << _version.c_str() << endl;
    cout << "glsl version: " << _glslVersion.c_str() << endl;
    
    cout << "extensions:   " << endl;
    int extCount;
    glGetIntegerv(GL_NUM_EXTENSIONS, &extCount);
    for (int i = 0; i < extCount; ++i) {
        const char *ext = (const char *)glGetStringi(GL_EXTENSIONS, i);
        _extensions.push_back(ext);
        cout << "\t" << ext << endl;
    }
}

void GLSystem::InitializeGL(void) {
    GetSystemInfo();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_MULTISAMPLE);
}

bool GLSystem::CreateTexture(TextureInfo &textureInfo, const string &imageFile, bool mipmaps) {
    if (imageFile.empty()) {
        cout << "GLSystem::CreateTexture image path is empty!" << endl;
        return false;
    }

    int width, height, nComponent;
    string fullpath = Utilities::GetAssetPath() + "/" + imageFile;
    uchar *data = stbi_load(fullpath.c_str(), &width, &height, &nComponent, 0);
    if (!data) {
        cout << "GLSystem::CreateTexture load image " << imageFile.c_str() << " failed!" << endl;
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
            cout << "GLSystem::CreateTexture image format not support!" << endl;
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
    textureInfo.type = texture2D;
    textureInfo.width = width;
    textureInfo.height = height;

    return true;
}

bool GLSystem::CreateHDRTexture(TextureInfo &textureInfo, const string &imageFile) {
    if (imageFile.empty()) {
        cout << "GLSystem::CreateHDRTexture image path is empty!" << endl;
        return false;
    }

    int width, height, nComponent;
    string fullpath = Utilities::GetAssetPath() + "/" + imageFile;
    float *data = stbi_loadf(fullpath.c_str(), &width, &height, &nComponent, 0);
    if (!data) {
        cout << "GLSystem::CreateHDRTexture load image " << imageFile.c_str() << " failed!" << endl;
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
    textureInfo.type = texture2D;
    textureInfo.width = width;
    textureInfo.height = height;

    return true;
}

bool GLSystem::ReadTextFile(string &content, const string &filePath) {
    if (filePath.empty()) {
        cout << "GLSystem::ReadTextFile file path is empty!" << endl;
        return false;
    }
    
    ifstream ifs(filePath.c_str());
    if (!ifs.is_open()) {
        cout << "Failed to open " << filePath.c_str() << endl;
        return false;
    }
    
    content.clear();
    content.assign(istreambuf_iterator<char>(ifs),
                   istreambuf_iterator<char>());
    ifs.close();

    return true;
}

bool GLSystem::LoadShader(uint &shader, uint type, string &source, string &header) {
    if (source.empty()) {
        cout << "GLSystem::LoadShader shader source is empty!" << endl;
        return false;
    }

    if (type != GL_VERTEX_SHADER && type != GL_FRAGMENT_SHADER) {
        cout << "GLSystem::LoadShader unknown shader type!" << endl;
        return false;
    }

    shader = glCreateShader(type);
    const char *data[2] = {header.c_str(), source.c_str()};
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

        cout << "GLSystem::LoadShader shader compile with error:" << log << endl;
        delete [] log;

        glDeleteShader(shader);
        shader = 0;
        return false;
    }

    return true;
}

bool GLSystem::CreateProgram(uint &program, const string &vsFile, const string &fsFile, const list<string> &macros) {
    if (vsFile.empty() || fsFile.empty()) {
        cout << "GLSystem::CreateProgram invalid shader file path!" << endl;
        return false;
    }

    // read file content
    string vsFullpath = Utilities::GetAssetPath() + "/" + vsFile;
    string fsFullpath = Utilities::GetAssetPath() + "/" + fsFile;
    string vsSource;
    string fsSource;
    if (!ReadTextFile(vsSource, vsFullpath) || !ReadTextFile(fsSource, fsFullpath)) {
        cout << "GLSystem::CreateProgram failed when read shader file!" << endl;
        return false;
    }

    // assemble file header
    string header(ShaderVersion);
    for (const string &macro : macros) { header.append(macro + '\n'); }

    uint vertexShader;
    uint fragmentShader;
    if (!LoadShader(vertexShader, GL_VERTEX_SHADER, vsSource, header)
        || !LoadShader(fragmentShader, GL_FRAGMENT_SHADER, fsSource, header)) {
        cout << "GLSystem::CreateProgram Load shader failed!" << endl;
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

        cout << "GLSystem::CreateProgram shader link with error:" << log << endl;
        delete [] log;

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(program);
        program = 0;
        return false;
    }

    return true;
}

bool GLSystem::CreateBufferObject(uint &object, const void *data, uint size, bool isElement) {
    if (data == nullptr || size == 0) {
        cout << "GLSystem::CreateBufferObject buffer data is empty!" << endl;
        return false;
    }

    uint target = isElement ? GL_ELEMENT_ARRAY_BUFFER : GL_ARRAY_BUFFER;

    glGenBuffers(1, &object);
    glBindBuffer(target, object);
    glBufferData(target, size, data, GL_STATIC_DRAW);

    glBindBuffer(target, 0);
    return true;
}

bool GLSystem::CreateBufferObject(uint &object, const vector<float> &data) {
    return CreateBufferObject(object, data.data(), (uint)(data.size() * sizeof(float)), false);
}

bool GLSystem::CreateBufferObject(uint &object, const vector<short> &data, bool isElement) {
    return CreateBufferObject(object, data.data(), (uint)(data.size() * sizeof(short)), isElement);
}

bool GLSystem::CreateVertexInfo(VertexInfo &vertex, const Mesh &mesh) {
    const Data<float> &vertices = mesh.vertices;
    const vector<short> &indices = mesh.indices;
    if (vertices.data.size() == 0) {
        cout << "GLSystem::CreateVertexObjects vertex data is empty!" << endl;
        return false;
    }

    CreateBufferObject(vertex.vbo, vertices.data);
    if (indices.size() > 0) {
        CreateBufferObject(vertex.ebo, indices, true);
    } else {
        vertex.ebo = 0;
    }

    glGenVertexArrays(1, &vertex.vao);
    glBindVertexArray(vertex.vao);
    glBindBuffer(GL_ARRAY_BUFFER, vertex.vbo);

    int szFloat = sizeof(float);

    int stride = 0;
    for (const DataLayout &layout : vertices.layouts) {
        switch(layout.type) {
            case Float: stride += szFloat; break;
            case Float2: stride += 2 * szFloat; break;
            case Float3: stride += 3 * szFloat; break;
            case Float4: stride += 4 * szFloat; break;
            default: cout << "Invalod layout data type of vertices, type:" << layout.type << endl; break;
        }
    }

    size_t offset = 0;
    for (const DataLayout &layout : vertices.layouts) {
        glEnableVertexAttribArray(layout.usage);
        int nComponent = 0;
        switch(layout.type) {
            case Float: nComponent = 1; break;
            case Float2: nComponent = 2; break;
            case Float3: nComponent = 3; break;
            case Float4: nComponent = 4; break;
            default: cout << "Invalod layout data type of vertices, type:" << layout.type << endl; break;
        }
        glVertexAttribPointer(layout.usage, nComponent, GL_FLOAT, GL_FALSE, stride, (const void *)offset);

        offset += nComponent * szFloat;
    }

    if (vertex.ebo != 0) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex.ebo);
    }

    glBindVertexArray(0);

    vertex.count = (uint)(vertex.ebo != 0 ? indices.size() : vertices.data.size() / (stride / szFloat));
    vertex.layouts = vertices.layouts;
    
    vertex.mode = mesh.mode;

    return true;
}

bool GLSystem::CheckError(void) {
    uint error = glGetError();
    switch (error) {
        case GL_NO_ERROR: cout << "CheckError: NO_ERROR" << endl; return true;
        case GL_INVALID_ENUM: cout << "CheckError: INVALID_ENUM" << endl; break;
        case GL_INVALID_VALUE: cout << "CheckError: INVALID_VALUE" << endl; break;
        case GL_INVALID_OPERATION: cout << "CheckError: INVALID_OPERATION" << endl; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: cout << "CheckError: INVALID_FRAMEBUFFER_OPERATION" << endl; break;
        case GL_OUT_OF_MEMORY: cout << "CheckError: OUT_OF_MEMORY" << endl; break;
        default: cout << "CheckError: Other Error:" << error << endl; break;
    }

    return false;
}

bool GLSystem::CheckFramebufferStatus(void) {
    uint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch(status) {
        case GL_FRAMEBUFFER_COMPLETE: 
            cout << "CheckFramebufferStatus: FRAMEBUFFER_COMPLETE" << endl; 
            return true;
        case GL_FRAMEBUFFER_UNDEFINED: 
            cout << "CheckFramebufferStatus: FRAMEBUFFER_UNDEFINED" << endl; 
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: 
            cout << "CheckFramebufferStatus: FRAMEBUFFER_INCOMPLETE_ATTACHMENT" << endl; 
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: 
            cout << "CheckFramebufferStatus: FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT" << endl; 
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: 
            cout << "CheckFramebufferStatus: FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER" << endl; 
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: 
            cout << "CheckFramebufferStatus: FRAMEBUFFER_INCOMPLETE_READ_BUFFER" << endl; 
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED: 
            cout << "CheckFramebufferStatus: FRAMEBUFFER_UNSUPPORTED" << endl; 
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: 
            cout << "CheckFramebufferStatus: FRAMEBUFFER_INCOMPLETE_MULTISAMPLE" << endl; 
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: 
            cout << "CheckFramebufferStatus: FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS" << endl; 
            break;
        default: 
            cout << "CheckFramebufferStatus: Frame Buffer status error:" << status << endl; 
            break;
    }

    return false;
}

}
