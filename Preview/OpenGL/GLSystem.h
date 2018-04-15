#ifndef MTP_GLSYSTEM_H
#define MTP_GLSYSTEM_H

#include <OpenGL/gl3.h>
#include "Interfaces.h"

namespace mtp {

class GLSystem {
    static const char *ShaderVersion;

public:
    explicit GLSystem(void);
    virtual ~GLSystem(void);

    void InitializeGL(void);

    bool CreateTexture(TextureInfo &textureInfo, const string &imageFile, bool mipmaps = true);
    // mipmaps is disabled when load  from HDR image
    bool CreateHDRTexture(TextureInfo &textureInfo, const string &imageFile);
    bool CreateProgram(uint &program, const string &vsFile, const string &fsFile, const list<string> &macros);
    bool CreateBufferObject(uint &object, const vector<float> &data);
    bool CreateBufferObject(uint &object, const vector<short> &data, bool isElement = true);
    bool CreateVertexInfo(VertexInfo &vertex, const Mesh &mesh);

    bool CheckError(void);
    bool CheckFramebufferStatus(void);

private:
    void GetSystemInfo(void);
    bool ReadTextFile(string &content, const string &filePath);
    // type:  GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
    bool LoadShader(uint &shader, uint type, string &source, string &header);
    bool CreateBufferObject(uint &object, const void *data, uint size, bool isElement);

    string _vendor;
    string _renderer;
    string _version;
    string _glslVersion;
    list<string> _extensions;
};

}

#endif // MTP_GLSYSTEM_H
