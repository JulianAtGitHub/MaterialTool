#ifndef MTP_PREPROCESSOR_H
#define MTP_PREPROCESSOR_H

#include "glm/mat4x4.hpp"
#include "Interfaces.h"
#include "GLSystem.h"
#include "Drawable.h"
#include "Utilities.h"

namespace mtp {
  
class PreProcessor {
    enum State {
        PrepareRendering,
        ConvertToEnvironmentMap,
        CalculateIrradianceMap,
        CalculatePreFilterMap,
        GenerateBRDFLookUpMap,
        Finished
    };
    
    const static uint CubeFaceCount = 6;

public:
    explicit PreProcessor(shared_ptr<GLSystem> glSystem);
    virtual ~PreProcessor(void);
    
    inline bool IsReady(void) const { return _state == Finished; }
    inline const TextureInfo & EnvMap(void) const { return _envCubeMap; }
    inline const TextureInfo & IrrMap(void) const { return _irrCubeMap; }
    inline const TextureInfo & FilMap(void) const { return _filCubeMap; }
    inline const TextureInfo & BrdfMap(void) const { return _brdfLutMap; }
    
    void Start(const string &sphereImage);
    void Update(void);

private:
    TextureInfo CreateCubeMap(uint size, bool mipmap = false);
    void RenderToCubeMap(uint cubemap, shared_ptr<Program> &program, uint mipLevel = 0);
    
    void ClearTextures(void);
    void PrepareRenderObjects(void);
    void RenderToEnvCubeMap(void);
    void RenderToIrrCubeMap(void);
    void RenderToFilCubeMap(void);
    void RenderToBRDFLutMap(void);
    void DeleteTempObjects(void);
    
    weak_ptr<GLSystem> _glSystem;
    State _state;
    glm::mat4 _viewProjMatrixes[CubeFaceCount];
    
    shared_ptr<Drawable> _unitCube;
    shared_ptr<Drawable> _unitQuad;
    
    shared_ptr<Program> _rad2envProgram;
    shared_ptr<Program> _env2irrProgram;
    shared_ptr<Program> _env2filProgram;
    shared_ptr<Program> _brdfGenProgram;
    
    uint _envSize;
    uint _irrSize;
    uint _filSize;
    uint _lutSize;
    
    TextureInfo _envCubeMap;
    TextureInfo _irrCubeMap;
    TextureInfo _filCubeMap;
    TextureInfo _brdfLutMap;
    
    // temprary objects, will be deleted after finished
    TextureInfo _sphereMap;
    uint _captureFBO;   // frame buffer object
    uint _captureCBO;   // color buffer object
    uint _captureDBO;   // depth buffer object
};
  
}

#endif // MTP_PREPROCESSOR_H
