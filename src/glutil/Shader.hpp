#pragma once

#include <string>
#include <stdexcept>
#include "_gl.hpp"

namespace GlUtil{
    class Shader final{
    public:
        Shader() noexcept;
        Shader(GLuint id) noexcept;

        //runtime_error if doesnt compiles 
        static Shader compile_new(GLenum type, const char *src);

        //logic_error if already created 
        void create(GLenum type);
        
        void source(GLsizei count, const GLchar *const *string, const GLint *length) noexcept;
        void source(const char *src) noexcept;
        void compile() noexcept;

        void get_iv(GLenum name, GLint *param) const noexcept;
        bool is_compiled() const noexcept;
        std::string info_log() const noexcept;
        GLuint id() const noexcept;

        void delete_shader() noexcept;
    private:
        GLuint _id;
    };
}