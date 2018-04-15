#include <cmath>
#include "glm/vec3.hpp"
#include "Utilities.h"

namespace mtp {

VertexInfo Utilities::Sphere;
VertexInfo Utilities::Skybox;
VertexInfo Utilities::Cube;
VertexInfo Utilities::Quad;
    
bool Utilities::ValidSphere = false;
bool Utilities::ValidSkybox = false;
bool Utilities::ValidCube = false;
bool Utilities::ValidQuad = false;

weak_ptr<GLSystem> Utilities::glSystem;
string Utilities::assetPath;

void Utilities::SetGLSystem(shared_ptr<GLSystem> &gl) {
    glSystem = gl;
}

shared_ptr<Program> Utilities::CreateSkyboxProgram(void) {
    ProgramDescription desc;
    desc.vsFile = "shaders/skybox.vs";
    desc.fsFile = "shaders/skybox.fs";
    desc.attribs = {"a_position"};
    desc.textures = {
        {"u_envCubeMap", textureCube}
    };
    desc.uniforms = {
        {"u_viewMatrix", Float4x4},
        {"u_projMatrix", Float4x4},
    };
    return make_shared<Program>(desc, glSystem.lock());
}
shared_ptr<Program> Utilities::CreatePbrProgram(bool isTextured) {
    ProgramDescription desc;
    desc.vsFile = "shaders/pbr.vs";
    desc.fsFile = "shaders/pbr.fs";
    desc.attribs = {"a_position", "a_normal", "a_texCoord"};
    if (isTextured) {
        desc.macros = {"#define HAS_PBR_TEXTURES"};
        desc.textures = {
            {"u_normalMap", texture2D},
            {"u_albedoMap", texture2D},
            {"u_metallicMap", texture2D},
            {"u_roughnessMap", texture2D},
            {"u_aoMap", texture2D},
            {"u_irradianceMap", textureCube},
            {"u_prefilterMap", textureCube},
            {"u_brdfMap", texture2D},
        };
        desc.uniforms = {
            {"u_modelMatrix", Float4x4},
            {"u_normalMatrix", Float3x3},
            {"u_viewProjMatrix", Float4x4},
            {"u_viewPos", Float3},
            {"u_lightPos", Float3},
            {"u_lightColor", Float3},
        };
    } else {
        desc.textures = {
            {"u_irradianceMap", textureCube},
            {"u_prefilterMap", textureCube},
            {"u_brdfMap", texture2D},
        };
        desc.uniforms = {
            {"u_modelMatrix", Float4x4},
            {"u_normalMatrix", Float3x3},
            {"u_viewProjMatrix", Float4x4},
            {"u_viewPos", Float3},
            {"u_lightPos", Float3},
            {"u_lightColor", Float3},
            {"u_albedo", Float3},
            {"u_metallic", Float},
            {"u_roughness", Float},
            {"u_ao", Float},
        };
    }
    return make_shared<Program>(desc, glSystem.lock());
}

shared_ptr<Program> Utilities::CreateDebugTexture2DProgram(void) {
    ProgramDescription desc;
    desc.vsFile = "shaders/debug/texture2d.vs";
    desc.fsFile = "shaders/debug/texture2d.fs";
    desc.attribs = {"a_position", "a_texCoord"};
    desc.textures = {
        {"u_texture2D", texture2D}
    };
    return make_shared<Program>(desc, glSystem.lock());
}

shared_ptr<Program> Utilities::CreateRad2EnvProgram(void) {
    ProgramDescription desc;
    desc.vsFile = "shaders/preprocessor/unitCube.vs";
    desc.fsFile = "shaders/preprocessor/rad2env.fs";
    desc.attribs = {"a_position"};
    desc.textures = {
        {"u_envSphereMap", texture2D},
    };
    desc.uniforms = {
        {"u_viewProjMatrix", Float4x4},
    };
    return make_shared<Program>(desc, glSystem.lock());
}

shared_ptr<Program> Utilities::CreateEnv2IrrProgram(void) {
    ProgramDescription desc;
    desc.vsFile = "shaders/preprocessor/unitCube.vs";
    desc.fsFile = "shaders/preprocessor/env2irr.fs";
    desc.attribs = {"a_position"};
    desc.textures = {
        {"u_envCubeMap", textureCube},
    };
    desc.uniforms = {
        {"u_viewProjMatrix", Float4x4},
    };
    return make_shared<Program>(desc, glSystem.lock());
}

shared_ptr<Program> Utilities::CreateEnv2FilProgram(void) {
    ProgramDescription desc;
    desc.vsFile = "shaders/preprocessor/unitCube.vs";
    desc.fsFile = "shaders/preprocessor/env2fil.fs";
    desc.attribs = {"a_position"};
    desc.textures = {
        {"u_envCubeMap", textureCube},
    };
    desc.uniforms = {
        {"u_viewProjMatrix", Float4x4},
        {"u_roughness", Float},
        {"u_resolution", Float},
    };
    return make_shared<Program>(desc, glSystem.lock());
}

shared_ptr<Program> Utilities::CreateBRDFGenProgram(void) {
    ProgramDescription desc;
    desc.vsFile = "shaders/preprocessor/brdf.vs";
    desc.fsFile = "shaders/preprocessor/brdf.fs";
    desc.attribs = {"a_position", "a_texCoord"};
    return make_shared<Program>(desc, glSystem.lock());
}

shared_ptr<Drawable> Utilities::CreateGoldenSphere(void) {
    Model model;
    model.mesh = GetSphere();
    model.textures = {
        {albedoMap, "gold_albedo.png"},
        {normalMap, "gold_normal.png"},
        {metallicMap, "gold_metallic.png"},
        {roughnessMap, "gold_roughness.png"},
        {aoMap, "gold_ao.png"},
    };
    
    return make_shared<Drawable>(model, glSystem.lock());
}

shared_ptr<Drawable> Utilities::CreateIronSphere(void) {
    Model model;
    model.mesh = GetSphere();
    model.textures = {
        {albedoMap, "iron_albedo.png"},
        {normalMap, "iron_normal.png"},
        {metallicMap, "iron_metallic.png"},
        {roughnessMap, "iron_roughness.png"},
        {aoMap, "iron_ao.png"},
    };
    
    return make_shared<Drawable>(model, glSystem.lock());
}

shared_ptr<Drawable> Utilities::CreatePlasticSphere(void) {
    Model model;
    model.mesh = GetSphere();
    model.textures = {
        {albedoMap, "plastic_albedo.png"},
        {normalMap, "plastic_normal.png"},
        {metallicMap, "plastic_metallic.png"},
        {roughnessMap, "plastic_roughness.png"},
        {aoMap, "plastic_ao.png"},
    };
    
    return make_shared<Drawable>(model, glSystem.lock());
}

shared_ptr<Drawable> Utilities::CreateNoTexturedSphere(void) {
    Model model;
    model.mesh = GetSphere();
    model.values = {
        {albedo, make_shared<glm::vec3>(1.0f)},
        {metallic, make_shared<float>(0.5f)},
        {roughness, make_shared<float>(0.5f)},
        {ao, make_shared<float>(0.5f)},
    };
    
    return make_shared<Drawable>(model, glSystem.lock());
}

const VertexInfo & Utilities::GetSphere(void) {
    if (!ValidSphere) {
        CreateSphere();
    }
    return Sphere;
}

const VertexInfo & Utilities::GetSkybox(void) {
    if (!ValidSkybox) {
        CreateSkybox();
    }
    return Skybox;
}

const VertexInfo & Utilities::GetCube(void) {
    if (!ValidCube) {
        CreateCube();
    }
    return Cube;
}

const VertexInfo & Utilities::GetQuad(void) {
    if (!ValidQuad) {
        CreateQuad();
    }
    return Quad;
}

void Utilities::CreateSphere(void) {
    const static uint X_SEGMENTS = 64;
    const static uint Y_SEGMENTS = 64;
    
    Mesh SphereMesh;

    // vertices
    SphereMesh.vertices.data.reserve((X_SEGMENTS + 1) * (Y_SEGMENTS + 1) * 8);
    for (uint y = 0; y <= Y_SEGMENTS; ++y) {
        for (uint x = 0; x <= X_SEGMENTS; ++x) {
            float xSegment = x / (float)X_SEGMENTS;
            float ySegment = y / (float)Y_SEGMENTS;
            float xPos = cos(xSegment * 2.0f * M_PI) * sin(ySegment * M_PI);
            float yPos = cos(ySegment * M_PI);
            float zPos = sin(xSegment * 2.0f * M_PI) * sin(ySegment * M_PI);
            // position
            SphereMesh.vertices.data.push_back(xPos);
            SphereMesh.vertices.data.push_back(yPos);
            SphereMesh.vertices.data.push_back(zPos);
            // normal
            SphereMesh.vertices.data.push_back(xPos);
            SphereMesh.vertices.data.push_back(yPos);
            SphereMesh.vertices.data.push_back(zPos);
            // texCoord
            SphereMesh.vertices.data.push_back(xSegment);
            SphereMesh.vertices.data.push_back(ySegment);
        }
    }
    SphereMesh.vertices.layouts = {
        {Float3, position},
        {Float3, normal},
        {Float2, texCoord},
    };
    
    // indices
    SphereMesh.indices.reserve(Y_SEGMENTS * (X_SEGMENTS + 1) * 2);
    bool oddRow = false;
    for (uint y = 0; y < Y_SEGMENTS; ++y) {
        if (!oddRow) {
            // even rows: y == 0, y == 2; and so on
            for (uint x = 0; x <= X_SEGMENTS; ++x) {
                SphereMesh.indices.push_back(y       * (X_SEGMENTS + 1) + x);
                SphereMesh.indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
            }
        } else {
            for (int x = X_SEGMENTS; x >= 0; --x) {
                SphereMesh.indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                SphereMesh.indices.push_back(y       * (X_SEGMENTS + 1) + x);
            }
        }
        oddRow = !oddRow;
    }
    
    SphereMesh.mode = TriangleStrip;
    
    if (!glSystem.expired()) {
        shared_ptr<GLSystem> system = glSystem.lock();
        system->CreateVertexInfo(Sphere, SphereMesh);
        ValidSphere = true;
    }
}

void Utilities::CreateSkybox(void) {
    Mesh SkyboxMesh;

    SkyboxMesh.vertices.data = {
        // Back face
        -1.0f, -1.0f, -1.0f, //0
        -1.0f,  1.0f, -1.0f, //1
         1.0f,  1.0f, -1.0f,  //2
         1.0f, -1.0f, -1.0f,  //3
        // Front face
        -1.0f, -1.0f,  1.0f, //4
         1.0f, -1.0f,  1.0f, //5
         1.0f,  1.0f,  1.0f, //6
        -1.0f,  1.0f,  1.0f, //7
        // Left face
        -1.0f, -1.0f, -1.0f, //8
        -1.0f, -1.0f,  1.0f, //9
        -1.0f,  1.0f,  1.0f, //10
        -1.0f,  1.0f, -1.0f, //11
        // Right face
         1.0f, -1.0f, -1.0f, //12
         1.0f,  1.0f, -1.0f, //13
         1.0f,  1.0f,  1.0f, //14
         1.0f, -1.0f,  1.0f, //15
        // Bottom face
        -1.0f, -1.0f, -1.0f, //16
         1.0f, -1.0f, -1.0f, //17
         1.0f, -1.0f,  1.0f, //18
        -1.0f, -1.0f,  1.0f, //19
        // Top face
        -1.0f,  1.0f, -1.0f, //20
        -1.0f,  1.0f,  1.0f, //21
         1.0f,  1.0f,  1.0f, //22
         1.0f,  1.0f, -1.0f, //23
    };
    
    SkyboxMesh.vertices.layouts.push_back({Float3, position});
    
    SkyboxMesh.indices = {
        0,  2,  3,    2,  0,  1,    // back
        4,  5,  6,    6,  7,  4,    // front
        10, 11, 8,    8,  9, 10,    // left
        14, 12, 13,   12, 14, 15,   // right
        16, 17, 18,   18, 19, 16,   // bottom
        20, 22, 23,   22, 20, 21,   // top
    };
    
    SkyboxMesh.mode = TriangleList;
    
    if (!glSystem.expired()) {
        shared_ptr<GLSystem> system = glSystem.lock();
        system->CreateVertexInfo(Skybox, SkyboxMesh);
        ValidSkybox = true;
    }
}

void Utilities::CreateCube(void) {
    Mesh CubeMesh;
    
    CubeMesh.vertices.data = {
        // Front face 
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
         1.0f, -1.0f,  1.0f,  1.0f,  0.0f,
         1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,
        // Back face
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
        -1.0f,  1.0f, -1.0f,  1.0f,  0.0f,
         1.0f,  1.0f, -1.0f,  1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,  0.0f,  1.0f,
        // Top face
        -1.0f,  1.0f, -1.0f,  0.0f,  0.0f,
        -1.0f,  1.0f,  1.0f,  1.0f,  0.0f,
         1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,  0.0f,  1.0f,
        // Bottom face
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
         1.0f, -1.0f, -1.0f,  1.0f,  0.0f,
         1.0f, -1.0f,  1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,  0.0f,  1.0f,
        // Right face
         1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
         1.0f,  1.0f, -1.0f,  1.0f,  0.0f,
         1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,  0.0f,  1.0f,
        // Left face
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
        -1.0f, -1.0f,  1.0f,  1.0f,  0.0f,
        -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,
    };
    
    CubeMesh.vertices.layouts = {
        {Float3, position},
        {Float2, texCoord},
    };
    
    CubeMesh.indices = {
        0,  1,  2,      0,  2,  3,    // front
        4,  5,  6,      4,  6,  7,    // back
        8,  9,  10,     8,  10, 11,   // top
        12, 13, 14,     12, 14, 15,   // bottom
        16, 17, 18,     16, 18, 19,   // right
        20, 21, 22,     20, 22, 23,   // left
    };
    
    CubeMesh.mode = TriangleList;
    
    if (!glSystem.expired()) {
        shared_ptr<GLSystem> system = glSystem.lock();
        system->CreateVertexInfo(Cube, CubeMesh);
        ValidCube = true;
    }
}

void Utilities::CreateQuad(void) {
    Mesh QuadMesh;
    
    QuadMesh.vertices.data = {
        -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
        -1.0f, -1.0f,  0.0f,  0.0f,  0.0f,
         1.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
    };
    
    QuadMesh.vertices.layouts = {
        {Float3, position},
        {Float2, texCoord},
    };
    
    QuadMesh.mode = TriangleStrip;
    
    if (!glSystem.expired()) {
        shared_ptr<GLSystem> system = glSystem.lock();
        system->CreateVertexInfo(Quad, QuadMesh);
        ValidQuad = true;
    }
}

}
