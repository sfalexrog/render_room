//
// Created by sf on 28/11/17.
//

#include "framebuffer.h"

namespace cmcray
{
    Framebuffer::Framebuffer(glm::ivec2 size) : _size(size)
    {
        gl::GLint previousFramebuffer;
        gl::glGetIntegerv(gl::GL_FRAMEBUFFER_BINDING, &previousFramebuffer);

        gl::glGenFramebuffers(1, &_framebufferId);
        gl::glBindFramebuffer(gl::GL_FRAMEBUFFER, _framebufferId);

        gl::glGenTextures(1, &_textureId);
        gl::glBindTexture(gl::GL_TEXTURE_2D, _textureId);

        gl::glTexImage2D(gl::GL_TEXTURE_2D, 0, gl::GL_RGB, _size.x, _size.y, 0, gl::GL_RGB, gl::GL_UNSIGNED_BYTE, 0);

        gl::glTexParameteri(gl::GL_TEXTURE_2D, gl::GL_TEXTURE_MIN_FILTER, gl::GL_LINEAR);
        gl::glTexParameteri(gl::GL_TEXTURE_2D, gl::GL_TEXTURE_MAG_FILTER, gl::GL_LINEAR);
        gl::glTexParameteri(gl::GL_TEXTURE_2D, gl::GL_TEXTURE_WRAP_R, gl::GL_CLAMP_TO_EDGE);
        gl::glTexParameteri(gl::GL_TEXTURE_2D, gl::GL_TEXTURE_WRAP_S, gl::GL_CLAMP_TO_EDGE);

        gl::glFramebufferTexture2D(gl::GL_FRAMEBUFFER, gl::GL_COLOR_ATTACHMENT0, gl::GL_TEXTURE_2D, _textureId, 0);

        gl::glGenRenderbuffers(1, &_renderbufferId);
        gl::glBindRenderbuffer(gl::GL_RENDERBUFFER, _renderbufferId);
        gl::glRenderbufferStorage(gl::GL_RENDERBUFFER, gl::GL_DEPTH24_STENCIL8, _size.x, _size.y);

        gl::glFramebufferRenderbuffer(gl::GL_FRAMEBUFFER, gl::GL_DEPTH_STENCIL_ATTACHMENT, gl::GL_RENDERBUFFER, _renderbufferId);

        gl::glBindFramebuffer(gl::GL_FRAMEBUFFER, previousFramebuffer);
    }

    Framebuffer::~Framebuffer()
    {
        gl::GLint currFramebuffer;
        gl::glGetIntegerv(gl::GL_FRAMEBUFFER, &currFramebuffer);
        if (currFramebuffer == _framebufferId)
        {
            gl::glBindFramebuffer(gl::GL_FRAMEBUFFER, 0);
        }

        gl::glDeleteTextures(1, &_textureId);
        gl::glDeleteRenderbuffers(1, &_renderbufferId);
        gl::glDeleteFramebuffers(1, &_framebufferId);
    }

    void Framebuffer::activate()
    {
        gl::glBindFramebuffer(gl::GL_FRAMEBUFFER, _framebufferId);
    }
}
