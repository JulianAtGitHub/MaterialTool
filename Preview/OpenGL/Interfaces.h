#ifndef MTP_INTERFACES_H
#define MTP_INTERFACES_H

#include <string>
#include <vector>
#include <list>
#include <map>
#include <memory>

#include "glm/vec3.hpp"

using namespace std;

namespace mtp {
    
typedef unsigned int uint;
typedef unsigned char uchar;

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
    Short,
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
    
    resolution,
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
    
    irradianceMap,
    prefilterMap,
    brdfMap,

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
    
template <class _Tp>
struct Data {
    vector<_Tp> data;
    list<DataLayout> layouts;
};

struct VertexInfo {
    uint vao;
    uint vbo;
    uint ebo;
    // Element count if ebo is exist
    // Vertex count if ebo is not exist
    uint count;
    PrimitiveType mode;
    list<DataLayout> layouts;
};

struct Mesh {
    Data<float> vertices;
    vector<short> indices;
    PrimitiveType mode;
};

struct Model {
    VertexInfo mesh;
    map<TextureUsage, string> textures;
    map<UniformUsage, shared_ptr<void> > values;
};

struct LightInfo {
    glm::vec3 position;
    glm::vec3 color;
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

}

#endif // MTP_INTERFACES_H
