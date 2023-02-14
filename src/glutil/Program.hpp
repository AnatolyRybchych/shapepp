#pragma once

#include "Shader.hpp"

namespace GlUtil{
    class Program final{
    public:
        Program() noexcept;
        Program(GLuint id) noexcept;

        static Program link_new(const Shader &first);
        static Program link_new(const Shader &first, const Shader &second);
        static Program link_new(const Shader &first, const Shader &second, const Shader &third);

        //logic_error if already created 
        void create();

        void attach(GLuint shader) noexcept;
        void attach(const Shader &shader) noexcept;
        void link() noexcept;

        void get_iv(GLenum name, GLint *param) const noexcept;
        bool is_linked() const noexcept;
        std::string info_log() const noexcept;
        GLuint id() const noexcept;

        void use() const noexcept;
        void unuse() const noexcept;
        GLint uniform_location(const char *name) const noexcept; 
        GLint attrib_location(const char *name) const noexcept;

        void delete_program() noexcept;
    private:
        static Program link_new(const Shader *shader, ...);
        GLuint _id;
    };
}