#include <iostream>
#include "Program.h"

namespace mtp {

map<DataUsage, string> Program::AttribUsage2Name;
map<UniformUsage, string> Program::UniformUsage2Name;
map<TextureUsage, string> Program::TextureUsage2Name;

map<string, DataUsage> Program::AttribName2Usage;
map<string, UniformUsage> Program::UniformName2Usage;
map<string, TextureUsage> Program::TextureName2Usage;

void Program::InitNameMappings(void) {
    AttribUsage2Name[position] = "a_position";
    AttribUsage2Name[normal] = "a_normal";
    AttribUsage2Name[texCoord] = "a_texCoord";
    AttribUsage2Name[color] = "a_color";
    AttribUsage2Name[tangent] = "a_tangent";
    AttribUsage2Name[bitangent] = "a_bitangent";

    UniformUsage2Name[viewProjMatrix] = "u_viewProjMatrix";
    UniformUsage2Name[modelMatrix] = "u_modelMatrix";
    UniformUsage2Name[normalMatrix] = "u_normalMatrix";
    UniformUsage2Name[viewMatrix] = "u_viewMatrix";
    UniformUsage2Name[projMatrix] = "u_projMatrix";
    UniformUsage2Name[viewPos] = "u_viewPos";
    UniformUsage2Name[lightPos] = "u_lightPos";
    UniformUsage2Name[lightColor] = "u_lightColor";
    UniformUsage2Name[albedo] = "u_albedo";
    UniformUsage2Name[metallic] = "u_metallic";
    UniformUsage2Name[roughness] = "u_roughness";
    UniformUsage2Name[ao] = "u_ao";
    UniformUsage2Name[resolution] = "u_resolution";

    TextureUsage2Name[normalMap] = "u_normalMap";
    TextureUsage2Name[diffuseMap] = "u_diffuseMap";
    TextureUsage2Name[specularMap] = "u_specularMap";
    TextureUsage2Name[albedoMap] = "u_albedoMap";
    TextureUsage2Name[metallicMap] = "u_metallicMap";
    TextureUsage2Name[roughnessMap] = "u_roughnessMap";
    TextureUsage2Name[aoMap] = "u_aoMap";
    TextureUsage2Name[irradianceMap] = "u_irradianceMap";
    TextureUsage2Name[prefilterMap] = "u_prefilterMap";
    TextureUsage2Name[brdfMap] = "u_brdfMap";
    TextureUsage2Name[envCubeMap] = "u_envCubeMap";
    TextureUsage2Name[envSphereMap] = "u_envSphereMap";
    TextureUsage2Name[debug2DMap] = "u_debug2DMap";
    TextureUsage2Name[debugCubeMap] = "u_debugCubeMap";

    AttribName2Usage["a_position"] = position;
    AttribName2Usage["a_normal"] = normal;
    AttribName2Usage["a_texCoord"] = texCoord;
    AttribName2Usage["a_color"] = color;
    AttribName2Usage["a_tangent"] = tangent;
    AttribName2Usage["a_bitangent"] = bitangent;

    UniformName2Usage["u_viewProjMatrix"] = viewProjMatrix;
    UniformName2Usage["u_modelMatrix"] = modelMatrix;
    UniformName2Usage["u_normalMatrix"] = normalMatrix;
    UniformName2Usage["u_viewMatrix"] = viewMatrix;
    UniformName2Usage["u_projMatrix"] = projMatrix;
    UniformName2Usage["u_viewPos"] = viewPos;
    UniformName2Usage["u_lightPos"] = lightPos;
    UniformName2Usage["u_lightColor"] = lightColor;
    UniformName2Usage["u_albedo"] = albedo;
    UniformName2Usage["u_metallic"] = metallic;
    UniformName2Usage["u_roughness"] = roughness;
    UniformName2Usage["u_ao"] = ao;
    UniformName2Usage["u_resolution"] = resolution;

    TextureName2Usage["u_normalMap"] = normalMap;
    TextureName2Usage["u_diffuseMap"] = diffuseMap;
    TextureName2Usage["u_specularMap"] = specularMap;
    TextureName2Usage["u_albedoMap"] = albedoMap;
    TextureName2Usage["u_metallicMap"] = metallicMap;
    TextureName2Usage["u_roughnessMap"] = roughnessMap;
    TextureName2Usage["u_aoMap"] = aoMap;
    TextureName2Usage["u_envCubeMap"] = envCubeMap;
    TextureName2Usage["u_envSphereMap"] = envSphereMap;
    TextureName2Usage["u_debug2DMap"] = debug2DMap;
    TextureName2Usage["u_debugCubeMap"] = debugCubeMap;
    TextureName2Usage["u_irradianceMap"] = irradianceMap;
    TextureName2Usage["u_prefilterMap"] = prefilterMap;
    TextureName2Usage["u_brdfMap"] = brdfMap;
}

Program::Program(ProgramDescription &description, shared_ptr<GLSystem> glSystem)
: _glSystem(glSystem)
, _glProgram(0)
{
    Initialize(description);
}

Program::~Program(void) {

}

void Program::Initialize(ProgramDescription &description) {
    if (description.vsFile.empty() || description.fsFile.empty()) {
        cout << "Program::Initialize shader file name is empty!" << endl;
        return;
    }

    if (_glSystem.expired()) {
        return;
    }

    shared_ptr<GLSystem> glSystem = _glSystem.lock();
    if (!(glSystem->CreateProgram(_glProgram, description.vsFile, description.fsFile, description.macros))) {
        cout << "Program::Initialize create program failed!" << endl;
        return;
    }

    uint texIndex = 0;
    for (const auto &item : description.textures) {
        const string &name = item.first;
        int location = glGetUniformLocation(_glProgram, name.c_str());
        if (location != -1) {
            TextureUsage usage = TextureName2Usage[name];
            ShaderTexture tex = {location, item.second, texIndex};
            _samplers[usage] = tex;
            ++texIndex;
        } else {
            cout << "Program::Initialize sampler name " << name.c_str() << " not exist!" << endl;
        }
    }

    for (const auto &item : description.uniforms) {
        const string &name = item.first;
        int location = glGetUniformLocation(_glProgram, name.c_str());
        if (location != -1) {
            UniformUsage usage = UniformName2Usage[name];
            ShaderUniform uni = {location, item.second};
            _uniforms[usage] = uni;
        } else {
            cout << "Program::Initialize uniform name " << name.c_str() << " not exist!" << endl;
        }
    }

    glGetProgramiv(_glProgram, GL_ACTIVE_ATTRIBUTES, &_attribCount);
    _attribs.resize(DataUsageMax);
    for (uint i = 0; i < DataUsageMax; ++i) { _attribs[i] = false; }
    for (const string &attrib : description.attribs) {
        int location = glGetAttribLocation(_glProgram, attrib.c_str());
        if (location != -1) {
            _attribs[location] = true;
        } else {
            cout << "Program::Initialize attribute name " << attrib.c_str() << " not exist!" << endl;
        }
    }

}

bool Program::Use(void) {
    if (_glProgram == 0) {
        return false;
    }

    glUseProgram(_glProgram);
    return true;
}

bool Program::SetTexture(TextureUsage usage, uint texture, TextureType type) {
    if (_samplers.find(usage) == _samplers.end()) {
        cout << "Program::SetTexture sampler uniform " << usage << " not exist!" << endl;
        return false;
    }

    ShaderTexture &tex = _samplers[usage];
    if (tex.type != type) {
        cout  << "Program::SetTexture texture " << usage << " type not matched!" << endl;
        return false;
    }

    glActiveTexture(GL_TEXTURE0 + tex.index);

    switch(tex.type) {
      case texture2D: glBindTexture(GL_TEXTURE_2D, texture); break;
      case textureCube: glBindTexture(GL_TEXTURE_CUBE_MAP, texture); break;
      default: cout << "Program::SetTexture Invalid texture type!" << endl; break;
    }

    glUniform1i(tex.location, tex.index);

    return true;
}

bool Program::SetUniform(UniformUsage usage, const void *value, DataType type) {
    if (value == nullptr) {
        return false;
    }

    if (_uniforms.find(usage) == _uniforms.end()) {
        cout << "Program::SetUniform uniform use for " << usage << " not exist!" << endl;
        return false;
    }

    ShaderUniform &uni = _uniforms[usage];
    if (uni.type != type) {
        cout << "Program::SetUniform uniform use for " << usage << " type not matched!" << endl;
        return false;
    }

    switch(uni.type) {
      case Float4x4: glUniformMatrix4fv(uni.location, 1, false, (const float *)value); break;
      case Float3x3: glUniformMatrix3fv(uni.location, 1, false, (const float *)value); break;
      case Float4: glUniform4fv(uni.location, 1, (const float *)value); break;
      case Float3: glUniform3fv(uni.location, 1, (const float *)value); break;
      case Float2: glUniform2fv(uni.location, 1, (const float *)value); break;
      case Float: glUniform1f(uni.location, *((const float *)value)); break;
      case Int: glUniform1i(uni.location, *((const int *)value)); break;
      default: cout << "Program::SetUniform Unknow uniform type:" << uni.type << endl; break;
    }

    return true;
}

bool Program::CheckAttribLocation(const list<DataLayout> &layouts) {
    if (layouts.size() < _attribCount) {
        return false;
    }

    for (const DataLayout &layout : layouts) {
        if (layout.usage >= DataUsageMax || _attribs[layout.usage] == false) {
            return false;
        }
    }

    return true;
}

}
