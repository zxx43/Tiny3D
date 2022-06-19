#include "textureBuffer.h"
#include "../util/util.h"

TextureBuffer::TextureBuffer(int p, int c, int size, void* data) :precision(p), component(c), maxSize(size) {
    switch (precision) {
        case LOW_PRE:
            bitSize = sizeof(byte);
            break;
        case HIGH_PRE:
            bitSize = sizeof(short);
            break;
        case HALF_PRE:
            bitSize = sizeof(half);
            break;
        case FLOAT_PRE:
            bitSize = sizeof(float);
            break;
    }
    GLenum useage = data ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW;

    glGenBuffers(1, &buffId);
    glBindBuffer(GL_TEXTURE_BUFFER, buffId);
    glBufferData(GL_TEXTURE_BUFFER, bitSize * component * maxSize, data, useage);
    glBindBuffer(GL_TEXTURE_BUFFER, 0);

    GLenum type = GL_RGBA32F;
    if (precision == FLOAT_PRE) {
        switch (component) {
            case 1:
                type = GL_R32F;
                break;
            case 2:
                type = GL_RG32F;
                break;
            case 3:
                type = GL_RGB32F;
                break;
            case 4:
                type = GL_RGBA32F;
                break;
        }
    } else if (precision == HALF_PRE) {
        switch (component) {
            case 1:
                type = GL_R16F;
                break;
            case 2:
                type = GL_RG16F;
                break;
            case 3:
                type = GL_RGB16F;
                break;
            case 4:
                type = GL_RGBA16F;
                break;
        }
    } else if (precision == HIGH_PRE) {
        switch (component) {
            case 1:
                type = GL_R16I;
                break;
            case 2:
                type = GL_RG16I;
                break;
            case 3:
                type = GL_RGB16I;
                break;
            case 4:
                type = GL_RGBA16I;
                break;
        }
    } else if (precision == LOW_PRE) {
        switch (component) {
            case 1:
                type = GL_R8I;
                break;
            case 2:
                type = GL_RG8I;
                break;
            case 3:
                type = GL_RGB8I;
                break;
            case 4:
                type = GL_RGBA8I;
                break;
        }
    }

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_BUFFER, id);
    glTexBuffer(GL_TEXTURE_BUFFER, type, buffId);
    glBindTexture(GL_TEXTURE_BUFFER, 0);

    hnd = genBindless();
}

TextureBuffer::~TextureBuffer() {
    releaseBindless(hnd);
    glDeleteTextures(1, &id);
    glDeleteBuffers(1, &buffId);
}

void TextureBuffer::update(uint size, void* data) {
    glNamedBufferSubData(buffId, 0, size * component * bitSize, data);
}

u64 TextureBuffer::genBindless() {
    u64 texHnd = glGetTextureHandleARB(id);
    glMakeTextureHandleResidentARB(texHnd);
    return texHnd;
}

void TextureBuffer::releaseBindless(u64 texHnd) {
    glMakeTextureHandleNonResidentARB(texHnd);
}