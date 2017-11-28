//
// Created by sf on 28/11/17.
//

#ifndef CMC_RAYMARCH_FRAMEBUFFER_H
#define CMC_RAYMARCH_FRAMEBUFFER_H

#include <glbinding/gl43core/gl.h>
#include <glm/vec2.hpp>


namespace cmcray
{

    class Framebuffer
    {
    private:
        gl::GLuint _framebufferId;
        gl::GLuint _renderbufferId;
        gl::GLuint _textureId;
        glm::vec2 _size;

    public:
        Framebuffer(glm::ivec2 size);
        ~Framebuffer();

        glm::ivec2 getSize() { return _size; }

        gl::GLuint getTex() { return _textureId; }

        void activate();
    };


    class FramebufferBinding
    {
    private:
        gl::GLint _prevBuffer;
        gl::GLint _vp[4];
    public:
        FramebufferBinding(Framebuffer& fb)
        {
            gl::glGetIntegerv(gl::GL_FRAMEBUFFER_BINDING, &_prevBuffer);
            gl::glGetIntegerv(gl::GL_VIEWPORT, _vp);
            fb.activate();
            auto fbSize = fb.getSize();
            gl::glViewport(0, 0, fbSize.x, fbSize.y);
        }
        ~FramebufferBinding()
        {
            gl::glBindFramebuffer(gl::GL_FRAMEBUFFER, _prevBuffer);
            gl::glViewport(_vp[0], _vp[1], _vp[2], _vp[3]);
        }
    };

}

#endif //CMC_RAYMARCH_FRAMEBUFFER_H
