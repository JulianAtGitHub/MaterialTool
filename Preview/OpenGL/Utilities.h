#ifndef MTP_UTILITIES_H
#define MTP_UTILITIES_H

#include "Interfaces.h"
#include "Drawable.h"
#include "Program.h"
#include "GLSystem.h"

namespace mtp {

class Utilities {
public:
    static void SetGLSystem(shared_ptr<GLSystem> &glSystem);
    inline static const string & GetAssetPath(void) { return assetPath; }
    inline static void SetAssetPath(const string &path) { assetPath = path; }

    static shared_ptr<Program> CreateSkyboxProgram(void);
    static shared_ptr<Program> CreatePbrProgram(bool isTextured = true);
    static shared_ptr<Program> CreateDebugTexture2DProgram(void);
    
    static shared_ptr<Program> CreateRad2EnvProgram(void);
    static shared_ptr<Program> CreateEnv2IrrProgram(void);
    static shared_ptr<Program> CreateEnv2FilProgram(void);
    static shared_ptr<Program> CreateBRDFGenProgram(void);

    static shared_ptr<Drawable> CreateGoldenSphere(void);
    static shared_ptr<Drawable> CreateIronSphere(void);
    static shared_ptr<Drawable> CreatePlasticSphere(void);
    static shared_ptr<Drawable> CreateNoTexturedSphere(void);

    static const VertexInfo & GetSphere(void);
    static const VertexInfo & GetSkybox(void);
    static const VertexInfo & GetCube(void);
    static const VertexInfo & GetQuad(void);

private:
    static void CreateSphere(void);
    static void CreateSkybox(void);
    static void CreateCube(void);
    static void CreateQuad(void);
    
    static VertexInfo Sphere;
    static VertexInfo Skybox;
    static VertexInfo Cube;
    static VertexInfo Quad;
    
    static bool ValidSphere;
    static bool ValidSkybox;
    static bool ValidCube;
    static bool ValidQuad;
    
    static weak_ptr<GLSystem> glSystem;
    static string assetPath;
};

}

#endif // MTP_UTILITIES_H
