#ifndef MTR_INTERFACES_H
#define MTR_INTERFACES_H

#include <QtGlobal>
#include <QList>
#include <QVector>

namespace mtr {

enum DataType {
    Unknown = 0,
    Float,
    Float2,
    Float3,
    Float4,
    Float2x2,
    Float3x3,
    Float4x4,
    Int,
    UInt
};

enum DataUsage {
    position = 0,
    normal,
    texCoord,
    color,
    index
};

enum UniformUsage {
    viewProjMatrix,
    modelMatrix,
    normalMatrix,
    viewMatrix,
    projMatrix,
    viewPos,
    lightPos,
    lightColor,
    albedo,
    metallic,
    roughness,
    ao,
};

enum TextureType {
    Texture2D,
    TextureCube,
};

struct TextureInfo {
    uint texture;
    TextureType type;
    uint width;
    uint height;
};

struct DataLayout {
    DataType type;
    DataUsage usage;
};

struct FloatData {
    QVector<float> data;
    QList<DataLayout> layouts;
};

struct ShortData {
    QVector<short> data;
    QList<DataLayout> layouts;
};

struct IntData {
    QVector<int> data;
    QList<DataLayout> layouts;
};

struct VertexInfo {
    uint vao;
    uint vbo;
    uint ebo;
    // Element count if ebo is exist
    // Vertex count if ebo is not exist
    uint count;
    QList<DataLayout> layouts;
};

struct ShaderUniform {
    int location;
    DataType type;
};

struct ShaderTexture {
    int location;
    DataType type;
    uint index;
};

}

#endif // MTR_INTERFACES_H
