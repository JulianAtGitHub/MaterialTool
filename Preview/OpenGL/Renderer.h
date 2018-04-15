#ifndef MTP_RENDERER_H
#define MTP_RENDERER_H

#include "Interfaces.h"
#include "GLSystem.h"
#include "Camera.h"
#include "Drawable.h"
#include "Program.h"

namespace mtp {

class Renderer {
public:
    explicit Renderer(shared_ptr<GLSystem> glSystem);
    virtual ~Renderer(void);
    
    void Clear(void);
    void Draw(Camera &camera, Drawable &drawable, LightInfo &light, Program &program);
    
private:
    void SetupUniforms(Camera &camera, Drawable &drawable, LightInfo &light, Program &program);

    weak_ptr<GLSystem> _glSystem;

};

}

#endif // MTP_RENDERER_H
