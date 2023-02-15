#include "Shape.hpp"
#include <math.h>
#include <array>
#include <iostream>
#include <algorithm>

static const std::array<char, 8> SHAPE_MAGIC{'S', 'H', 'A', 'P', 'E', ' ', '\n', '\0'};

Shape::Shape(std::size_t width, std::size_t height) noexcept{
    this->width = width;
    this->height = height;
    this->fragments.resize(width * height);
    for(auto &frag:fragments){
        frag = -INFINITY;
    }
}


Shape::Shape(FILE *stream){
    std::array<char, 8> magic;
    fread(&magic[0], 1, magic.size(), stream);

    if(magic != SHAPE_MAGIC){
        throw std::invalid_argument("shape magic mismatch");
    }

    uint32_t w, h;
    fread(&w, sizeof(w), 1, stream);
    fread(&h, sizeof(h), 1, stream);
    width = w;
    height = h;

    this->fragments.resize(width * height);

    for(auto &frag:fragments){
        fread(&frag, sizeof(frag), 1, stream);
    }

    if(ferror(stream)){
        throw std::runtime_error("cannot read stream completely");
    }
}

std::size_t Shape::get_width() const noexcept{
    return this->width;
}

std::size_t Shape::get_height() const noexcept{
    return this->height;
}





Shape::Renderer::Renderer() noexcept{
    _is_init = false;
    rel_to_width = false;
}

Shape::Renderer::~Renderer() noexcept{
    try{
        uninit();
    }
    catch(std::logic_error &){}
}

void Shape::Renderer::init(){
    if(is_init()){
        throw std::logic_error("already initialized");
    }

    GlUtil::Shader vert = GlUtil::Shader::compile_new(
    GL_VERTEX_SHADER,
    R"GLSL(
        #version 110

        attribute vec4 v_pos;
        
        varying vec2 f_pos;
        varying vec2 f_uvpos;

        void main(){
            f_pos = v_pos.xy;
            f_uvpos = v_pos.xy * vec2(0.5) + vec2(0.5);
            gl_Position = v_pos;
        }
    )GLSL");

    GlUtil::Shader frag = GlUtil::Shader::compile_new(
    GL_FRAGMENT_SHADER,
    R"GLSL(
        #version 110

        uniform vec4 f_color;
        uniform float f_power;
        uniform sampler2D f_shape;

        varying vec2 f_pos;
        varying vec2 f_uvpos;

        void main(){
            float mask = clamp(texture2D(f_shape, f_uvpos).r * f_power, -1.0, 1.0) * f_color.a;
            gl_FragColor = vec4(f_color.rgb, mask);
        }
    )GLSL");
    prog_render = GlUtil::Program::link_new(vert, frag);
    vert.delete_shader();
    frag.delete_shader();

    pr_v_pos = prog_render.attrib_location("v_pos");
    pr_f_color = prog_render.uniform_location("f_color");
    pr_f_power = prog_render.uniform_location("f_power");
    pr_f_shape = prog_render.uniform_location("f_shape");

    _is_init = true;
}

void Shape::Renderer::uninit(){
    if(is_init()){
        prog_render.delete_program();

        _is_init = false;
    }
    else{
        throw std::logic_error("is not init yet");
    }
}

bool Shape::Renderer::is_init() const noexcept{
    return _is_init;
}

void Shape::Renderer::render(GLuint shape_texture, const std::array<float, 4> color, float power) const noexcept{
    if(!is_init()) return;

    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);

    float actual_power;
    if(rel_to_width) actual_power = vp[2] * power;
    else actual_power = vp[3] * power;

    prog_render.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, shape_texture);

    glUniform1i(pr_f_shape, 0);
    glUniform4f(pr_f_color, color[0], color[1], color[2], color[3]);
    glUniform1f(pr_f_power, actual_power);
    
    glEnableVertexAttribArray(pr_v_pos);
    float vertices[] = {
        -1.0, 1.0, 1.0, 1.0, 1.0, -1.0,
        -1.0, 1.0, -1.0, -1.0, 1.0, -1.0,
    };
    glVertexAttribPointer(pr_v_pos, 2, GL_FLOAT, GL_FALSE, 0, vertices);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(pr_v_pos);
    prog_render.unuse();
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Shape::Renderer::shape_texture(const Shape &shape, GLuint &texture) const noexcept{
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, shape.get_width(), shape.get_height(), 0, GL_RED, GL_FLOAT, shape.fragments.data());
    glBindTexture(GL_TEXTURE_2D, 0);
}
