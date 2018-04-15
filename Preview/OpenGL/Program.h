#ifndef MTP_PROGRAM_H
#define MTP_PROGRAM_H

#include "Interfaces.h"
#include "GLSystem.h"

namespace mtp {

struct ProgramDescription {
    string vsFile;
    string fsFile;
    list<string> macros;
    list<string> attribs;
    map<string, TextureType> textures;
    map<string, DataType> uniforms;
};

class Program {
    static map<DataUsage, string> AttribUsage2Name;
    static map<UniformUsage, string> UniformUsage2Name;
    static map<TextureUsage, string> TextureUsage2Name;

    static map<string, DataUsage> AttribName2Usage;
    static map<string, UniformUsage> UniformName2Usage;
    static map<string, TextureUsage> TextureName2Usage;

public:
    static void InitNameMappings(void);

    explicit Program(ProgramDescription &description, shared_ptr<GLSystem> glSystem);
    virtual ~Program(void);

    inline bool Valid(void) const { return _glProgram != 0; }
    inline const map<TextureUsage, ShaderTexture> & Samplers(void) const { return _samplers; }
    inline const map<UniformUsage, ShaderUniform> & Uniforms(void) const { return _uniforms; }

    bool Use(void);
    bool SetTexture(TextureUsage usage, uint texture, TextureType type);
    bool SetUniform(UniformUsage usage, const void *value, DataType type);
    bool CheckAttribLocation(const list<DataLayout> &layouts);

private:
    void Initialize(ProgramDescription &description);

    weak_ptr<GLSystem> _glSystem;
    uint _glProgram;
    int _attribCount;
    vector<bool> _attribs;
    map<TextureUsage, ShaderTexture> _samplers;
    map<UniformUsage, ShaderUniform> _uniforms;
};

}

#endif // MTP_PROGRAM_H
