#ifndef MTP_PREVIEW_H
#define MTP_PREVIEW_H

#include "GLSystem.h"
#include "Renderer.h"
#include "Program.h"
#include "Camera.h"
#include "Drawable.h"
#include "PreProcessor.h"

namespace mtp {
  
class Preview {
public:
    explicit Preview(uint viewWidth, uint viewHeight, const string &resourcePath);
    virtual ~Preview(void);
    
    void Initialize(void);
    void Update(float deltaTime);
    
    // handle input
    void OnMouseDown(float x, float y);
    void OnMouseUp(float x, float y);
    void OnMouseDrag(float deltaX, float deltaY);
    
private:
    void SetIBLTextureToDrawable(shared_ptr<Drawable> &drawable);

    uint _width;
    uint _height;
    
    shared_ptr<GLSystem> _glSystem;
    shared_ptr<Renderer> _renderer;
    shared_ptr<PreProcessor> _preProcessor;
    
    shared_ptr<Program> _pbrProgram;
    shared_ptr<Program> _pbrNTProgram;  // pbr no textured
    shared_ptr<Program> _skyboxProgram;
    
    shared_ptr<Drawable> _goldenSphere;
    shared_ptr<Drawable> _plasticSphere;
    shared_ptr<Drawable> _ironSphere;
    shared_ptr<Drawable> _mutableSphere;
    shared_ptr<Drawable> _skybox;
    
    LightInfo _light;
    shared_ptr<Camera> _camera;
    bool _pbrTexturesSetted;
};
  
}

#endif // MTP_PREVIEW_H
