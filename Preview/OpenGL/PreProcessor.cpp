#include <cmath>
#include <iostream>
#include "glm/vec3.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "PreProcessor.h"
#include "Utilities.h"

namespace mtp {

#define TO_RADIANS(degree) (degree * M_PI / 180.0)
#define IS_POT(value) ((value & (value - 1)) == 0)

PreProcessor::PreProcessor(shared_ptr<GLSystem> glSystem)
: _glSystem(glSystem)
, _state(PrepareRendering)
, _envSize(1024)
, _irrSize(32)
, _filSize(128)
, _lutSize(512)
, _captureFBO(0)
, _captureCBO(0)
, _captureDBO(0)
{
    _envCubeMap.texture = 0;
    _irrCubeMap.texture = 0;
    _filCubeMap.texture = 0;
    _brdfLutMap.texture = 0;
    _sphereMap.texture = 0;
    
    glm::mat4 projection = glm::perspectiveRH((float)TO_RADIANS(90.0), 1.0f, 0.1f, 10.0f);
    _viewProjMatrixes[0] = projection * glm::lookAtRH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f, 0.0f, 0.0f), glm::vec3(0.0f,-1.0f, 0.0f));
    _viewProjMatrixes[1] = projection * glm::lookAtRH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f,-1.0f, 0.0f));
    _viewProjMatrixes[2] = projection * glm::lookAtRH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    _viewProjMatrixes[3] = projection * glm::lookAtRH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,-1.0f, 0.0f), glm::vec3(0.0f, 0.0f,-1.0f));
    _viewProjMatrixes[4] = projection * glm::lookAtRH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, 0.0f, 1.0f), glm::vec3(0.0f,-1.0f, 0.0f));
    _viewProjMatrixes[5] = projection * glm::lookAtRH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, 0.0f,-1.0f), glm::vec3(0.0f,-1.0f, 0.0f));
    
    PrepareRenderObjects();
}

PreProcessor::~PreProcessor(void) {

}

void PreProcessor::Start(const string &sphereImage) {
    if (_glSystem.expired()) {
        return;
    }

    shared_ptr<GLSystem> glSystem = _glSystem.lock();
    if (!glSystem->CreateHDRTexture(_sphereMap, sphereImage)) {
        cout << "PreProcessor::Start load hdr texture:" << sphereImage << " failed!" << endl;
        return;
    }
    
    ClearTextures();

    _state = ConvertToEnvironmentMap;
}

void PreProcessor::Update(void) {
    switch (_state) {
        case ConvertToEnvironmentMap: {
            if (_rad2envProgram->Valid()) {
                RenderToEnvCubeMap();
            }
            break;
        }
        case CalculateIrradianceMap: {
            if (_env2irrProgram->Valid()) {
                RenderToIrrCubeMap();
            }
            break;
        }
        case CalculatePreFilterMap: {
            if (_env2filProgram->Valid()) {
                RenderToFilCubeMap();
            }
            break;
        }
        case GenerateBRDFLookUpMap: {
            if (_brdfGenProgram->Valid()) {
                RenderToBRDFLutMap();
            }
            break;
        }
        default:
            break;
    }
}

TextureInfo PreProcessor::CreateCubeMap(uint size, bool mipmap) {
    uint level = 0;
    uint internalFormat = GL_RGBA16F;
    uint border = 0;
    uint srcFormat = GL_RGBA;
    uint srcType = GL_FLOAT;
    
    uint cubeMap = 0;
    glGenTextures(1, &cubeMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
    for (uint i = 0; i < CubeFaceCount; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, level, internalFormat, size, size, border, srcFormat, srcType, 0);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (mipmap && IS_POT(size)) {
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return {cubeMap, textureCube, size, size};
}

void PreProcessor::RenderToCubeMap(uint cubemap, shared_ptr<Program> &program, uint mipLevel) {
    const VertexInfo &vertexInfo = _unitCube->Vertex();
    glBindVertexArray(vertexInfo.vao);

    for (uint i = 0; i < CubeFaceCount; ++i) {
      program->SetUniform(viewProjMatrix, &(_viewProjMatrixes[i]), Float4x4);
      glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemap, mipLevel);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glDrawElements(GL_TRIANGLES, vertexInfo.count, GL_UNSIGNED_SHORT, 0);
    }

    glBindVertexArray(0);
}

void PreProcessor::ClearTextures(void) {
    if (_envCubeMap.texture) { 
        glDeleteTextures(1, &(_envCubeMap.texture)); 
        _envCubeMap.texture = 0;
    }
    
    if (_irrCubeMap.texture) { 
        glDeleteTextures(1, &(_irrCubeMap.texture)); 
        _irrCubeMap.texture = 0;
    }

    if (_filCubeMap.texture) { 
        glDeleteTextures(1, &(_filCubeMap.texture)); 
        _filCubeMap.texture = 0;
    }
    
    if (_brdfLutMap.texture) {
        glDeleteTextures(1, &(_brdfLutMap.texture)); 
         _brdfLutMap.texture = 0;
    }
}

void PreProcessor::PrepareRenderObjects(void) {
    _rad2envProgram = Utilities::CreateRad2EnvProgram();
    _env2irrProgram = Utilities::CreateEnv2IrrProgram();
    _env2filProgram = Utilities::CreateEnv2FilProgram();
    _brdfGenProgram = Utilities::CreateBRDFGenProgram();
    
    Model unitCube;
    unitCube.mesh = Utilities::GetSkybox();
    _unitCube = make_shared<Drawable>(unitCube, _glSystem.lock());
    
    Model unitQuad;
    unitQuad.mesh = Utilities::GetQuad();
    _unitQuad = make_shared<Drawable>(unitQuad, _glSystem.lock());
}

void PreProcessor::RenderToEnvCubeMap(void) {
    glGenFramebuffers(1, &_captureFBO);
    glGenRenderbuffers(1, &_captureCBO);
    glGenRenderbuffers(1, &_captureDBO);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _captureFBO);

    glBindRenderbuffer(GL_RENDERBUFFER, _captureCBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA16F, _envSize, _envSize);
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _captureCBO);
    glBindRenderbuffer(GL_RENDERBUFFER, _captureDBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, _envSize, _envSize);
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _captureDBO);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

//    glClearColor(0.0, 0.0, 0.0, 1.0);
//    glClearDepth(1.0);
//    glEnable(GL_DEPTH_TEST);
//    glDepthFunc(GL_LEQUAL);

    _envCubeMap = CreateCubeMap(_envSize);
    _rad2envProgram->Use();
    _rad2envProgram->SetTexture(envSphereMap, _sphereMap.texture, _sphereMap.type);

    glViewport(0, 0, _envSize, _envSize);
    RenderToCubeMap(_envCubeMap.texture, _rad2envProgram);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    glBindTexture(GL_TEXTURE_CUBE_MAP, _envCubeMap.texture);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    _state = CalculateIrradianceMap;
}

void PreProcessor::RenderToIrrCubeMap(void) {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _captureFBO);

    glBindRenderbuffer(GL_RENDERBUFFER, _captureCBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA16F, _irrSize, _irrSize);
    glBindRenderbuffer(GL_RENDERBUFFER, _captureDBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, _irrSize, _irrSize);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    _irrCubeMap = CreateCubeMap(_irrSize);
    _env2irrProgram->Use();
    _env2irrProgram->SetTexture(envCubeMap, _envCubeMap.texture, textureCube);

    glViewport(0, 0, _irrSize, _irrSize);
    RenderToCubeMap(_irrCubeMap.texture, _env2irrProgram);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    _state = CalculatePreFilterMap;
}

void PreProcessor::RenderToFilCubeMap(void) {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _captureFBO);

    _filCubeMap = CreateCubeMap(_filSize, true);

    _env2filProgram->Use();
    float envSize = _envSize;
    _env2filProgram->SetUniform(resolution, &envSize, Float);
    _env2filProgram->SetTexture(envCubeMap, _envCubeMap.texture, textureCube);

    uint maxMipmapLevels = 5;
    uint size = _filSize;
    for (uint mipmap = 0; mipmap < maxMipmapLevels; ++mipmap) {
        glBindRenderbuffer(GL_RENDERBUFFER, _captureCBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA16F, size, size);
        glBindRenderbuffer(GL_RENDERBUFFER, _captureDBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size, size);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        float roughness = (float)mipmap / (float)(maxMipmapLevels - 1);
        _env2filProgram->SetUniform(UniformUsage::roughness, &roughness, Float);

        glViewport(0, 0, size, size);
        RenderToCubeMap(_filCubeMap.texture, _env2filProgram, mipmap);

        size /= 2;
    }

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    _state = GenerateBRDFLookUpMap;
}

void PreProcessor::RenderToBRDFLutMap(void) {
    glGenTextures(1, &(_brdfLutMap.texture));
    glBindTexture(GL_TEXTURE_2D, _brdfLutMap.texture);

    uint level = 0;
    uint internalFormat = GL_RG16F;
    uint border = 0;
    uint srcFormat = GL_RG;
    uint srcType = GL_FLOAT;
    glTexImage2D(GL_TEXTURE_2D, level, internalFormat, _lutSize, _lutSize, border, srcFormat, srcType, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _captureFBO);

    glBindRenderbuffer(GL_RENDERBUFFER, _captureCBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RG16F, _lutSize, _lutSize);
    glBindRenderbuffer(GL_RENDERBUFFER, _captureDBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, _lutSize, _lutSize);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _brdfLutMap.texture, 0);

    glViewport(0, 0, _lutSize, _lutSize);

    _brdfGenProgram->Use();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const VertexInfo &vertexInfo = _unitQuad->Vertex();
    glBindVertexArray(vertexInfo.vao);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, vertexInfo.count);

    glBindVertexArray(0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    DeleteTempObjects();

    _state = Finished;
}

void PreProcessor::DeleteTempObjects(void) {
    glDeleteFramebuffers(1, &_captureFBO);
    glDeleteRenderbuffers(1, &_captureCBO);
    glDeleteRenderbuffers(1, &_captureDBO);

    _captureFBO = 0;
    _captureCBO = 0;
    _captureDBO = 0;
    
    if (_sphereMap.texture) { 
        glDeleteTextures(1, &(_sphereMap.texture)); 
        _sphereMap.texture = 0;
    }
}

}
