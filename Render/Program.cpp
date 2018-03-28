#include "Program.h"

namespace mtr {

QMap<DataUsage, QString> Program::AttribUsage2Name;
QMap<UniformUsage, QString> Program::UniformUsage2Name;
QMap<QString, DataUsage> Program::AttribName2Usage;
QMap<QString, UniformUsage> Program::UniformName2Usage;

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
}

Program::Program(ProgramDescription &description, QSharedPointer<GLSystem> &glSystem, QObject *parent)
: QObject(parent)
, _glSystemRef(glSystem)
, _glProgram(0)
{
    Initialize(description);
}

Program::~Program(void) {

}

void Program::Initialize(ProgramDescription &description) {
    if (!_glSystemRef) {
        return;
    }

    QSharedPointer<GLSystem> glSystem = _glSystemRef.toStrongRef();

    if (description.vsFile.isEmpty() || description.fsFile.isEmpty()) {
        qCritical("Program::Initialize shader file name is empty!");
        return;
    }


    if (!(glSystem->CreateProgram(_glProgram, description.vsFile, description.fsFile, description.macros))) {
        qCritical("Program::Initialize create program failed!");
        return;
    }

    {
        uint texIndex = 0;
        QMapIterator<QString, TextureType> iter(description.textures);
        while(iter.hasNext()) {
            iter.next();
            const QString &name = iter.key();
            QByteArray cname = name.toUtf8();
            int location = glSystem->glGetUniformLocation(_glProgram, cname.data());
            if (location != -1) {
                ShaderTexture tex = {location, iter.value(), texIndex};
                _samplers[cname] = tex;
                ++texIndex;
            } else {
                qWarning("Program::Initialize sampler name %s not exist!", cname.data());
            }
        }
    }

    {
        QMapIterator<QString, DataType> iter(description.uniforms);
        while(iter.hasNext()) {
            iter.next();
            const QString &name = iter.key();
            QByteArray cname = name.toUtf8();
            int location = glSystem->glGetUniformLocation(_glProgram, cname.data());
            if (location != -1) {
                UniformUsage usage = UniformName2Usage[name];
                ShaderUniform uni = {location, iter.value()};
                _uniforms[usage] = uni;
            } else {
                qWarning("Program::Initialize uniform name %s not exist!", cname.data());
            }
        }
    }

    glSystem->glGetProgramiv(_glProgram, GL_ACTIVE_ATTRIBUTES, &_attribCount);
    _attribs.resize(DataUsageMax);
    for (uint i = 0; i < DataUsageMax; ++i) { _attribs[i] = false; }
    foreach (const QString &attrib, description.attribs) {
        QByteArray name = attrib.toUtf8();
        int location = glSystem->glGetAttribLocation(_glProgram, name.data());
        if (location != -1) {
            _attribs[location] = true;
        } else {
            qWarning("Program::Initialize attribute name %s not exist!", name.data());
        }
    }

}

bool Program::Use(void) {
    if (_glProgram == 0) {
        return false;
    }

    if (!_glSystemRef) {
        return false;
    }

    QSharedPointer<GLSystem> glSystem = _glSystemRef.toStrongRef();

    glSystem->glUseProgram(_glProgram);

    return true;
}

bool Program::SetTexture(QString &name, uint texture, TextureType type) {
    if (name.isEmpty()) {
        return false;
    }

    QByteArray cname = name.toUtf8();
    if (_samplers.find(name) == _samplers.end()) {
        qWarning("Program::SetTexture sampler uniform %s not exist!", cname.data());
        return false;
    }

    ShaderTexture &tex = _samplers[name];
    if (tex.type != type) {
        qWarning("Program::SetTexture texture %s type not matched!", cname.data());
        return false;
    }

    if (!_glSystemRef) {
        return false;
    }

    QSharedPointer<GLSystem> glSystem = _glSystemRef.toStrongRef();
    glSystem->glActiveTexture(GL_TEXTURE0 + tex.index);

    switch(tex.type) {
      case Texture2D: glSystem->glBindTexture(GL_TEXTURE_2D, texture); break;
      case TextureCube: glSystem->glBindTexture(GL_TEXTURE_CUBE_MAP, texture); break;
      default: qWarning("Program::SetTexture Invalid texture type!"); break;
    }

    glSystem->glUniform1i(tex.location, tex.index);

    return true;
}

bool Program::SetUniform(UniformUsage usage, void *value, DataType type) {
    if (value == nullptr) {
        return false;
    }

    if (_uniforms.find(usage) == _uniforms.end()) {
        qWarning("Program::SetUniform uniform use for %d not exist!", usage);
        return false;
    }

    ShaderUniform &uni = _uniforms[usage];
    if (uni.type != type) {
        qWarning("Program::SetUniform uniform use for %d type not matched!", usage);
        return false;
    }

    if (!_glSystemRef) {
        return false;
    }

    QSharedPointer<GLSystem> glSystem = _glSystemRef.toStrongRef();

    switch(uni.type) {
      case Float4x4: glSystem->glUniformMatrix4fv(uni.location, 1, false, (float *)value); break;
      case Float3x3: glSystem->glUniformMatrix3fv(uni.location, 1, false, (float *)value); break;
      case Float4: glSystem->glUniform4fv(uni.location, 1, (float *)value); break;
      case Float3: glSystem->glUniform3fv(uni.location, 1, (float *)value); break;
      case Float2: glSystem->glUniform2fv(uni.location, 1, (float *)value); break;
      case Float: glSystem->glUniform1f(uni.location, *((float *)value)); break;
      case Int: glSystem->glUniform1i(uni.location, *((int *)value)); break;
      default: qWarning("Program::SetUniform Unknow uniform type:%d", uni.type); break;
    }

    return true;
}

bool Program::CheckAttribLocation(QList<int> &locations) {
    if (locations.count() < _attribCount) {
        return false;
    }

    foreach (int location, locations) {
        if (location >= DataUsageMax || _attribs[location] == false) {
            return false;
        }
    }

    return true;
}

}
