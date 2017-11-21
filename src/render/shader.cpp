#include <glbinding/gl43core/gl.h>
#include <vector>
#include <util/Logger.h>

#include <glm/gtc/type_ptr.hpp>
#include "shader.h"

namespace cmcray
{

    namespace
    {
        inline gl::GLenum shaderType(Shader::Type in_type)
        {
            switch(in_type)
            {
                case Shader::Type::Vertex:
                    return gl::GL_VERTEX_SHADER;
                case Shader::Type::Fragment:
                    return gl::GL_FRAGMENT_SHADER;
                case Shader::Type::Compute:
                    return gl::GL_COMPUTE_SHADER;
            }
        }

        inline const char* shaderTypeName(Shader::Type in_type)
        {
            switch(in_type)
            {
                case Shader::Type::Vertex:
                    return "Vertex";
                case Shader::Type::Fragment:
                    return "Fragment";
                case Shader::Type::Compute:
                    return "Compute";
            }
        }
    }

    Shader& Shader::activate()
    {
        gl::glUseProgram(_handle);
        return *this;
    }

    Shader& Shader::source(const char *source, Type type)
    {
        gl::GLint *pshader = nullptr;
        gl::GLenum glType;
        switch(type)
        {
            case Type::Vertex:
                pshader = &_vertexHandle;
                glType = gl::GL_VERTEX_SHADER;
                break;
            case Type::Fragment:
                pshader = &_fragmentHandle;
                glType = gl::GL_FRAGMENT_SHADER;
                break;
            case Type::Compute:
                pshader = &_computeHandle;
                glType = gl::GL_FRAGMENT_SHADER;
        }
        *pshader = gl::glCreateShader(shaderType(type));
        gl::glShaderSource(*pshader, 1, &source, nullptr);
        gl::glCompileShader(*pshader);
        gl::GLboolean compileStatus;
        gl::glGetShaderiv(*pshader, gl::GL_COMPILE_STATUS, &compileStatus);
        if (compileStatus == gl::GL_TRUE)
        {
            Log(VERBOSE) << shaderTypeName(type) << " shader compilation successful";
            gl::glAttachShader(_handle, *pshader);
        }
        else
        {
            gl::GLint logLength;
            gl::glGetShaderiv(*pshader, gl::GL_INFO_LOG_LENGTH, &logLength);

            std::vector<char> log(logLength);
            gl::glGetShaderInfoLog(*pshader, logLength, nullptr, log.data());
            Log(ERROR) << shaderTypeName(type) << " shader could not be compiled due to errors";
            Log(ERROR) << "Error log: ";
            Log(ERROR) << log.data();

            gl::glDeleteShader(*pshader);
            *pshader = -1;
        }
        return *this;
    }

    Shader& Shader::link()
    {
        gl::GLboolean linkStatus;

        gl::glLinkProgram(_handle);
        gl::glGetProgramiv(_handle, gl::GL_LINK_STATUS, &linkStatus);
        if (linkStatus == gl::GL_TRUE)
        {
            gl::glBindFragDataLocation(_handle, 0, "outColor");
            gl::GLint numActiveAttribs, numActiveUniforms, attrMaxLength, unifMaxLength;
            Log(VERBOSE) << "Program linkage successful, performing introspection";
            gl::glGetProgramiv(_handle, gl::GL_ACTIVE_ATTRIBUTES, &numActiveAttribs);
            gl::glGetProgramiv(_handle, gl::GL_ACTIVE_UNIFORMS, &numActiveUniforms);
            Log(VERBOSE) << "Active attributes: " << numActiveAttribs << ", active uniforms: " << numActiveUniforms;
            if (numActiveAttribs)
            {
                gl::glGetProgramiv(_handle, gl::GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &attrMaxLength);
                std::vector<char> attrName(attrMaxLength);
                for(int i = 0; i < numActiveAttribs; ++i)
                {
                    gl::GLint size;
                    gl::GLenum type;
                    gl::glGetActiveAttrib(_handle, i, attrMaxLength, nullptr, &size, &type, attrName.data());
                    gl::GLint location = gl::glGetAttribLocation(_handle, attrName.data());
                    Log(VERBOSE) << "Attribute " << i << ": size: " << size << ", name: " << attrName.data()
                                 << ", location: " << location;
                }
            }
            if (numActiveUniforms)
            {
                gl::glGetProgramiv(_handle, gl::GL_ACTIVE_UNIFORM_MAX_LENGTH, &unifMaxLength);
                std::vector<char> unifName(unifMaxLength);
                for(int i = 0; i < numActiveUniforms; ++i)
                {
                    gl::GLint size;
                    gl::GLenum type;
                    gl::glGetActiveUniform(_handle, i, unifMaxLength, nullptr, &size, &type, unifName.data());
                    gl::GLint location = gl::glGetUniformLocation(_handle, unifName.data());
                    Log(VERBOSE) << "Uniform " << i << ": size: " << size << ", name: " << unifName.data()
                                 << ", location: " << location;
                }
            }

            Log(VERBOSE) << "Marking shaders for deletion by garbage collector";
            if (_vertexHandle > -1) { gl::glDeleteShader(_vertexHandle); }
            if (_fragmentHandle > -1) { gl::glDeleteShader(_fragmentHandle); }
            if (_computeHandle > -1) { gl::glDeleteShader(_computeHandle); }
        }
        else
        {
            gl::GLint logLength;
            gl::glGetProgramiv(_handle, gl::GL_INFO_LOG_LENGTH, &logLength);
            std::vector<char> log(logLength);
            gl::glGetProgramInfoLog(_handle, logLength, nullptr, log.data());
            Log(ERROR) << "Could not link program due to errors";
            Log(ERROR) << "Error log: ";
            Log(ERROR) << log.data();
        }
        return *this;
    }

    Shader& Shader::bind(gl::GLint location, float value)
    {
        activate();
        gl::glUniform1f(location, value);
        return *this;
    }

    Shader& Shader::bind(gl::GLint location, const glm::mat4& matrix)
    {
        activate();
        gl::glUniformMatrix4fv(location, 1, gl::GL_FALSE, glm::value_ptr(matrix));
        return *this;
    }

    gl::GLint Shader::attrLoc(const char *attrName) {
        return gl::glGetAttribLocation(_handle, attrName);
    }

    Shader& Shader::bind(gl::GLint location, const glm::vec3& value)
    {
        gl::glUniform3fv(location, 1, glm::value_ptr(value));
        return *this;
    }

    Shader& Shader::bind(gl::GLint location, const glm::vec4& value)
    {
        gl::glUniform4fv(location, 1, glm::value_ptr(value));
        return *this;
    }

}

