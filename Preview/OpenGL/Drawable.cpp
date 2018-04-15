#include <iostream>
#include "Drawable.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"

namespace mtp {

Drawable::Drawable(const Model &model, shared_ptr<GLSystem> glSystem)
: _isDirty(true)
, _position(0.0f, 0.0f, 0.0f)
, _rotation(0.0f, 0.0f, 0.0f)
, _scale(1.0f, 1.0f, 1.0f)
{
    _vertex = model.mesh;
    _values = model.values;
    LoadTextures(model, glSystem);
}

Drawable::~Drawable(void) {

}

void Drawable::LoadTextures(const Model &model, shared_ptr<GLSystem> &glSystem) {
    for (const auto &item : model.textures) {
        TextureUsage usage = item.first;
        const string &name = item.second;
        TextureInfo texture;
        if (glSystem->CreateTexture(texture, name)) {
            _textures[usage] = texture;
        } else {
            cout << "Drawable::LoadTextures load texture " << name.c_str() << " failed!" << endl;
        }
    }
}

bool Drawable::GetTexture(TextureInfo &texture, const TextureUsage usage) {
    if (_textures.find(usage) == _textures.end()) {
        return false;
    }
    
    texture = _textures[usage];
    return true;
}

const void * Drawable::GetValue(const UniformUsage usage) {
    if (_values.find(usage) == _values.end()) {
        return nullptr;
    }
    
    return _values[usage].get();
}

void Drawable::CalculateMatrixes(void) {
    _modelMatrix = glm::translate(glm::mat4(), _position);
    
    _modelMatrix = glm::rotate(_modelMatrix, _rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    _modelMatrix = glm::rotate(_modelMatrix, _rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    _modelMatrix = glm::rotate(_modelMatrix, _rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    
    _modelMatrix = glm::scale(_modelMatrix, _scale);

    _normalMatrix = glm::mat3(glm::inverseTranspose(_modelMatrix));
}

const glm::mat4 & Drawable::ModelMatrix(void) {
    if (_isDirty) {
        CalculateMatrixes();
        this->_isDirty = false;
    }
    return _modelMatrix;
}

const glm::mat3 & Drawable::NormalMatrix(void) {
    if (_isDirty) {
        CalculateMatrixes();
        this->_isDirty = false;
    }
    return _normalMatrix;
}

void Drawable::Move(const glm::vec3 &t) {
    _position += t;
    this->_isDirty = true;
}

void Drawable::Move(float x, float y, float z) {
    _position.x += x;
    _position.y += y;
    _position.z += z;
    this->_isDirty = true;
}

void Drawable::Rotate(const glm::vec3 &r) {
    _rotation += r;
    this->_isDirty = true;
}

void Drawable::Rotate(float x, float y, float z) {
    _rotation.x += x;
    _rotation.y += y;
    _rotation.z += z;
    this->_isDirty = true;
}

void Drawable::Scale(const glm::vec3 &s) {
    _scale += s;
    this->_isDirty = true;
}

void Drawable::Scale(float x, float y, float z) {
    _scale.x += x;
    _scale.y += y;
    _scale.z += z;
    this->_isDirty = true;
}

}
