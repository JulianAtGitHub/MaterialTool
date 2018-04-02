#ifndef MTR_INTERFACES_H
#define MTR_INTERFACES_H

#include <QtGlobal>
#include <QString>
#include <QList>
#include <QVector>
#include <QByteArray>
#include <QMap>

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
    UInt,
    DataTypeMax
};

enum DataUsage {
    position = 0,
    normal,
    texCoord,
    color,
    tangent,
    bitangent,
    DataUsageMax
};

enum UniformUsage {
    viewProjMatrix = 0,
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
    UniformUsageMax
};

enum TextureType {
    texture2D = 0,
    textureCube,
    TextureTypeMax
};

enum TextureUsage {
    normalMap = 0,

    diffuseMap,
    specularMap,

    albedoMap,
    metallicMap,
    roughnessMap,
    aoMap,

    envCubeMap,
    envSphereMap,

    debug2DMap,
    debugCubeMap,
    TextureUsageMax
};

enum PrimitiveType {
    TriangleList = 0,
    TriangleStrip,
    TriangleFan,
    PrimitiveTypeMax
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
    PrimitiveType mode;
    QList<DataLayout> layouts;
};

struct ShaderUniform {
    int location;
    DataType type;
};

struct ShaderTexture {
    int location;
    TextureType type;
    uint index;
};

struct Mesh {
    FloatData vertices;
    ShortData indices;
    PrimitiveType mode;
};

struct Model {
    Mesh mesh;
    QMap<TextureUsage, QString> textures;
    QMap<UniformUsage, QByteArray> values;
};

}

#endif // MTR_INTERFACES_H
