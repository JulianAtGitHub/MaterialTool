#ifndef MTP_DRAWABLE_H
#define MTP_DRAWABLE_H

#include "glm/vec3.hpp"
#include "glm/mat3x3.hpp"
#include "glm/mat4x4.hpp"

#include "Interfaces.h"
#include "GLSystem.h"

namespace mtp {

class Drawable {
public:
    explicit Drawable(const Model &model, shared_ptr<GLSystem> glSystem);
    virtual ~Drawable(void);

    inline const glm::vec3 & Position(void) const { return _position; }
    inline const glm::vec3 & Rotation(void) const { return _rotation; }
    inline const glm::vec3 & Scale(void) const { return _scale; }
    const glm::mat4 & ModelMatrix(void);
    const glm::mat3 & NormalMatrix(void);

    void Move(const glm::vec3 &t);
    void Move(float x, float y, float z);
    void Rotate(const glm::vec3 &r);
    void Rotate(float x, float y, float z);
    void Scale(const glm::vec3 &s);
    void Scale(float x, float y, float z);
    
    inline const VertexInfo & Vertex(void) const { return _vertex; }
    bool GetTexture(TextureInfo &texture, const TextureUsage usage);
    inline void SetTexture(const TextureInfo &texture, const TextureUsage usage) { _textures[usage] = texture; }
    const void * GetValue(const UniformUsage usage);

private:
    void LoadTextures(const Model &model, shared_ptr<GLSystem> &glSystem);
    void CalculateMatrixes(void);

    map<TextureUsage, TextureInfo> _textures;
    map<UniformUsage, shared_ptr<void> > _values;
    VertexInfo _vertex;
    glm::vec3 _position;
    glm::vec3 _rotation;
    glm::vec3 _scale;
    glm::mat4 _modelMatrix;
    glm::mat3 _normalMatrix;
    bool _isDirty;
};

}

#endif // MTP_DRAWABLE_H
