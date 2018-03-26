#ifndef MTR_INTERFACES_H
#define MTR_INTERFACES_H

#include<QOpenGLContext>

namespace mtr {

enum TextureType {
    Texture2D,
    TextureCube,
};

struct TextureInfo {
    uint texture;
    TextureType type;
    uint width;
    uint height;
};

}

#endif // MTR_INTERFACES_H
