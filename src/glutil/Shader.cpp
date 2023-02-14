#include "Shader.hpp"

namespace GlUtil{

Shader::Shader() noexcept{
    _id = 0;
}

Shader::Shader(GLuint id) noexcept{
    _id = id;
}

Shader Shader::compile_new(GLenum type, const char *src){
    Shader result;
    result.create(type);
    result.source(src);
    result.compile();
    if(result.is_compiled()){
        return result;
    }
    else{
        throw std::runtime_error(result.info_log());
    }
}

void Shader::create(GLenum type){
    if(_id){
        throw std::logic_error("shader is already created");
    }
    else{
        _id = glCreateShader(type);
    }
}

void Shader::source(GLsizei count, const GLchar *const *string, const GLint *length) noexcept{
    glShaderSource(_id, count, string, length);
}

void Shader::source(const char *src) noexcept{
    source(1, &src, nullptr);
}

void Shader::compile() noexcept{
    glCompileShader(_id);
}

void Shader::get_iv(GLenum name, GLint *param) const noexcept{
    glGetShaderiv(_id, name, param);
}

bool Shader::is_compiled() const noexcept{
    GLint status;
    get_iv(GL_COMPILE_STATUS, &status);
    return status == GL_TRUE;
}

std::string Shader::info_log() const noexcept{
    std::string result;
    GLint log_len = 0;
    get_iv(GL_INFO_LOG_LENGTH, &log_len);
    
    if(log_len){
        result.resize(log_len);
        glGetShaderInfoLog(_id, log_len, nullptr, &result[0]);
    }

    return result;
}

void Shader::delete_shader() noexcept{
    glDeleteShader(_id);
    _id = 0;
}

GLuint Shader::id() const noexcept{
    return _id;
}


}
