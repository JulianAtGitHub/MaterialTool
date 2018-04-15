#include "Preview.h"
#include "stb_image.h"
#include "Utilities.h"

namespace mtp {

Preview::Preview(uint viewWidth, uint viewHeight, const string &resourcePath)
: _width(viewWidth)
, _height(viewHeight)
{
    string assetPath = resourcePath + "/assets";
    Utilities::SetAssetPath(assetPath);
}

Preview::~Preview(void) {

}

void Preview::Initialize(void) {
    stbi_set_flip_vertically_on_load(true);
    Program::InitNameMappings();
    
    _light = {
        glm::vec3(-10.0f, 10.0f, 10.0f),
        glm::vec3(300.0f, 300.0f, 300.0f)
    };

    _glSystem = make_shared<GLSystem>();
    _glSystem->InitializeGL();
    
    Utilities::SetGLSystem(_glSystem);
    
    _pbrProgram = Utilities::CreatePbrProgram();
    _pbrNTProgram = Utilities::CreatePbrProgram(false);
    _skyboxProgram = Utilities::CreateSkyboxProgram();
    
    _goldenSphere = Utilities::CreateGoldenSphere();
    _plasticSphere = Utilities::CreatePlasticSphere();
    _ironSphere = Utilities::CreateIronSphere();
    _mutableSphere = Utilities::CreateNoTexturedSphere();
    
    _goldenSphere->Move(-1.1f, 1.1f, -8.0f);
    _plasticSphere->Move(-1.1f, -1.1f, -8.0f);
    _ironSphere->Move(1.1f, -1.1f, -8.0f);
    _mutableSphere->Move(1.1f, 1.1f, -8.0f);

    Model skybox;
    skybox.mesh = Utilities::GetSkybox();
    _skybox = make_shared<Drawable>(skybox, _glSystem);
    
    _camera = make_shared<Camera>((45.0f * M_PI / 180.0f), (float)_width / (float)_height);

    _renderer = make_shared<Renderer>(_glSystem);
    _preProcessor = make_shared<PreProcessor>(_glSystem);
    
    _pbrTexturesSetted = false;
    string hdrFile = "Mans_Outside_2k.hdr";
    _preProcessor->Start(hdrFile);
}

void Preview::Update(float deltaTime) {
    if (deltaTime < 0.0f) { deltaTime = 0.0f; }
    
    if (_preProcessor->IsReady()) {
    
        if (!_pbrTexturesSetted) {
            SetIBLTextureToDrawable(_goldenSphere);
            SetIBLTextureToDrawable(_plasticSphere);
            SetIBLTextureToDrawable(_ironSphere);
            SetIBLTextureToDrawable(_mutableSphere);
            _skybox->SetTexture(_preProcessor->EnvMap(), envCubeMap);
            
            _pbrTexturesSetted = true;

            glViewport(0, 0, _width, _height);
        }
        
        _ironSphere->Rotate(0.0f, deltaTime * 0.1f, 0.0f);
        _goldenSphere->Rotate(0.0f, deltaTime * 0.1f, 0.0f);
        _plasticSphere->Rotate(0.0f, deltaTime * 0.1f, 0.0f);
        
        _renderer->Clear();
        _renderer->Draw(*_camera, *_ironSphere, _light, *_pbrProgram);
        _renderer->Draw(*_camera, *_goldenSphere, _light, *_pbrProgram);
        _renderer->Draw(*_camera, *_plasticSphere, _light, *_pbrProgram);
        _renderer->Draw(*_camera, *_mutableSphere, _light, *_pbrNTProgram);
        _renderer->Draw(*_camera, *_skybox, _light, *_skyboxProgram);
    } else {
        _preProcessor->Update();
    }
    
}

void Preview::SetIBLTextureToDrawable(shared_ptr<Drawable> &drawable) {
    drawable->SetTexture(_preProcessor->IrrMap(), irradianceMap);
    drawable->SetTexture(_preProcessor->FilMap() , prefilterMap);
    drawable->SetTexture(_preProcessor->BrdfMap(), brdfMap);
}

void Preview::OnMouseDown(float x, float y) {
    _camera->OnMouseDown(x, y);
}

void Preview::OnMouseUp(float x, float y) {
    _camera->OnMouseUp();
}

void Preview::OnMouseDrag(float deltaX, float deltaY) {
    _camera->OnMouseDrag(deltaX, deltaY);
}

}
