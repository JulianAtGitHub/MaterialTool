#include <iostream>
#include "Renderer.h"

namespace mtp {

Renderer::Renderer(shared_ptr<GLSystem> glSystem)
: _glSystem(glSystem)
{

}

Renderer::~Renderer(void) {

}

void Renderer::SetupUniforms(Camera &camera, Drawable &drawable, LightInfo &light, Program &program) {
    for (const auto &item : program.Uniforms()) {
        UniformUsage usage = item.first;
        const void *value = nullptr;
        DataType type = Unknown;
        switch (usage) {
            case viewProjMatrix: value = &(camera.ViewPorjMatrix()); type = Float4x4; break;
            case modelMatrix: value = &(drawable.ModelMatrix()); type = Float4x4; break;
            case normalMatrix: value = &(drawable.NormalMatrix()); type = Float3x3; break;
            case viewMatrix: value = &(camera.ViewMatrix()); type = Float4x4; break;
            case projMatrix: value = &(camera.ProjMatrix()); type = Float4x4; break;
            case viewPos: value = &(camera.Eye()); type = Float3; break;
            case lightPos: value = &(light.position); type = Float3; break;
            case lightColor: value = &(light.color); type = Float3; break;
            case albedo: value = drawable.GetValue(albedo); type = Float3; break;
            case metallic: value = drawable.GetValue(metallic); type = Float; break;
            case roughness: value = drawable.GetValue(roughness); type = Float; break;
            case ao: value = drawable.GetValue(ao); type = Float; break;
            default: break;
        }

        if (value != nullptr && type != Unknown) {
            program.SetUniform(usage, value, type);
        } else {
            cout << "Uniform " << usage << " not found in drawable!" << endl;
        }
    }
}

void Renderer::Clear(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::Draw(Camera &camera, Drawable &drawable, LightInfo &light, Program &program) {
    if (!program.Valid()) { return; }
    
    program.Use();

    const VertexInfo &vertexInfo = drawable.Vertex();

    // set attributes
    if (!program.CheckAttribLocation(vertexInfo.layouts)) {
        cout << "Check drawable vertex info failed!" << endl;
    }
    glBindVertexArray(vertexInfo.vao);

    // set textures
    for (const auto &item: program.Samplers()) {
        TextureUsage usage = item.first;
        TextureInfo texture;
        if (drawable.GetTexture(texture, usage)) {
            program.SetTexture(usage, texture.texture, texture.type);
        } else {
            cout << "Drawable texture:" << usage << " not exsit" << endl;
        }
    }

    SetupUniforms(camera, drawable, light, program);

    uint mode = GL_TRIANGLES;
    switch(vertexInfo.mode) {
        case TriangleStrip: mode = GL_TRIANGLE_STRIP; break;
        case TriangleFan: mode = GL_TRIANGLE_FAN; break;
        default: break;
    }

    if (vertexInfo.ebo) {
        uint vertexCount = vertexInfo.count;
        uint type = GL_UNSIGNED_SHORT;
        void *offset = 0;
        glDrawElements(mode, vertexCount, type, offset);
    } else {
        uint vertexCount = vertexInfo.count;
        uint first = 0;
        glDrawArrays(mode, first, vertexCount);
    }

    glBindVertexArray(0);
}

}
