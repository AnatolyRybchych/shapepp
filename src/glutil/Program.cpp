#include "Program.hpp"
#include <stdarg.h>

namespace GlUtil{

Program::Program() noexcept{
    _id = 0;
}

Program::Program(GLuint id) noexcept{
    _id = id;
}

Program Program::link_new(const Shader *shader,...){
    Program result;
    result.create();

    va_list s;
    va_start(s, shader);

    do{
        result.attach(*shader);
        shader = va_arg(s, const Shader *);
    }
    while(shader);

    result.link();

    if(result.is_linked()){
        return result;
    }
    else{
        throw std::runtime_error(result.info_log());
    }
}

Program Program::link_new(const Shader &first){
    return link_new(&first);
}

Program Program::link_new(const Shader &first, const Shader &second){
    return link_new(&first, &second);
}

Program Program::link_new(const Shader &first, const Shader &second, const Shader &third){
    return link_new(&first, &second, &third);
}

//logic_error if already created 
void Program::create(){
    if(_id == 0){
        _id = glCreateProgram();
    }
    else{
        throw std::logic_error("shader program is already created");
    }
}

void Program::attach(GLuint shader) noexcept{
    glAttachShader(_id, shader);
}

void Program::attach(const Shader &shader) noexcept{
    glAttachShader(_id, shader.id());
}

void Program::link() noexcept{
    glLinkProgram(_id);
}

void Program::get_iv(GLenum name, GLint *param) const noexcept{
    glGetProgramiv(_id, name, param);
}

bool Program::is_linked() const noexcept{
    GLint status;
    get_iv(GL_LINK_STATUS, &status);
    return status == GL_TRUE;
}

std::string Program::info_log() const noexcept{
    int log_len;
    std::string result;

    get_iv(GL_INFO_LOG_LENGTH, &log_len);
    result.resize(log_len);

    glGetProgramInfoLog(_id, log_len, nullptr, &result[0]);

    return result;
}

GLuint Program::id() const noexcept{
    return _id;
}

void Program::use() const noexcept{
    glUseProgram(_id);
}

void Program::unuse() const noexcept{
    glUseProgram(0);
}

GLint Program::uniform_location(const char *name) const noexcept{
    if(name == nullptr){
        return -1;
    }
    else{
        return glGetUniformLocation(_id, name);
    }
}

GLint Program::attrib_location(const char *name) const noexcept{
    if(name == nullptr){
        return -1;
    }
    else{
        return glGetAttribLocation(_id, name);
    }
}


void Program::delete_program() noexcept{
    glDeleteProgram(_id);
    _id = 0;
}

}