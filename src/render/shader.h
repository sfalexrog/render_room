#ifndef CMC_RAYMARCH_SHADER_H
#define CMC_RAYMARCH_SHADER_H

#include <glbinding/gl43core/gl.h>
#include <glm/mat4x4.hpp>

namespace cmcray
{

    // Shamelessly ripped off from Glitter
    class Shader
    {
    public:
        enum class Type
        {
            Vertex,
            Fragment,
            Compute
        };

    private:
        gl::GLuint _handle;
        // Per-shader handles for cleanup after linkage
        gl::GLint _vertexHandle, _fragmentHandle, _computeHandle;

    public:
        Shader() : _vertexHandle(-1), _fragmentHandle(-1), _computeHandle(-1) { _handle = gl::glCreateProgram(); }
        ~Shader() { gl::glDeleteProgram(_handle); }
        Shader& activate();
        Shader& source(const char* source, Type type);
        Shader& link();

        Shader& bind(gl::GLint location, float value);
        Shader& bind(gl::GLint location, const glm::mat4& matrix);

        Shader& bind(gl::GLint location, const glm::vec3& value);
        Shader& bind(gl::GLint location, const glm::vec4& value);


        gl::GLint attrLoc(const char* attrName);
    };
}

#endif //CMC_RAYMARCH_SHADER_H
