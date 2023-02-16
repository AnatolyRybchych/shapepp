#include "Shape.hpp"
#include <math.h>
#include <array>
#include <iostream>
#include <algorithm>
#include <string.h>
#include <errno.h>
#include <glm/glm.hpp>

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
    init_from_stream(stream);   
}

Shape::Shape(const char *file){
    FILE *f = fopen(file, "rb");

    if(f){
        try{
            init_from_stream(f);
        }
        catch (std::exception &){
            fclose(f);
            throw;
        }
        fclose(f);   
    }
    else{
        throw std::runtime_error(strerror(errno));
    }
}

Shape::Shape(std::vector<uint8_t> data){
    FILE *f = fmemopen(data.data(), data.size(), "rb");

    if(f){
        try{
            init_from_stream(f);
        }
        catch (std::exception &){
            fclose(f);
            throw;
        }
        fclose(f);
    }
    else{
        throw std::runtime_error(strerror(errno));
    }
}

std::size_t Shape::get_width() const noexcept{
    return this->width;
}

std::size_t Shape::get_height() const noexcept{
    return this->height;
}

void Shape::init_from_stream(FILE *stream){
    std::array<char, 8> magic{};
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
        uniform mat4 v_mvp;
        
        varying vec2 f_pos;
        varying vec2 f_uvpos;

        void main(){
            vec4 pos = v_pos * v_mvp;
            f_uvpos = v_pos.xy * vec2(0.5) + vec2(0.5);
            f_pos = pos.xy;
            gl_Position = pos;
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

    frag.delete_shader();
    frag = GlUtil::Shader::compile_new(
    GL_FRAGMENT_SHADER,
    R"GLSL(
        #version 110

        uniform vec4 f_color;
        uniform float f_power;
        uniform sampler2D f_shape1;
        uniform sampler2D f_shape2;
        uniform float f_progress;

        varying vec2 f_pos;
        varying vec2 f_uvpos;

        void main(){
            float shape = mix(texture2D(f_shape1, f_uvpos).r, texture2D(f_shape2, f_uvpos).r, f_progress);
            float mask = clamp(shape * f_power, -1.0, 1.0) * f_color.a;
            gl_FragColor = vec4(f_color.rgb, mask);
        }
    )GLSL");
    
    prog_morph = GlUtil::Program::link_new(vert, frag);

    vert.delete_shader();
    frag.delete_shader();

    pr_v_pos = prog_render.attrib_location("v_pos");
    pr_v_mvp = prog_render.uniform_location("v_mvp");
    pr_f_color = prog_render.uniform_location("f_color");
    pr_f_power = prog_render.uniform_location("f_power");
    pr_f_shape = prog_render.uniform_location("f_shape");

    pm_v_pos = prog_morph.attrib_location("v_pos");
    pm_v_mvp = prog_morph.uniform_location("v_mvp");
    pm_f_color = prog_morph.uniform_location("f_color");
    pm_f_power = prog_morph.uniform_location("f_power");
    pm_f_shape1 = prog_morph.uniform_location("f_shape1");
    pm_f_shape2 = prog_morph.uniform_location("f_shape2");
    pm_f_progress = prog_morph.uniform_location("f_progress");

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

void Shape::Renderer::render(GLuint shape_texture, glm::vec4 &color, float power, glm::mat4 &mvp) const noexcept{
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
    glUniform4f(pr_f_color, color.r, color.g, color.b, color.a);
    glUniform1f(pr_f_power, actual_power);
    glUniformMatrix4fv(pr_v_mvp, 1, GL_FALSE, &mvp[0][0]);
    
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

void Shape::Renderer::render_morph(GLuint shape_texture1, GLuint shape_texture2, glm::vec4 &color , float power, float progress, glm::mat4 &mvp) const noexcept{
    if(!is_init()) return;

    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);

    float actual_power;
    if(rel_to_width) actual_power = vp[2] * power;
    else actual_power = vp[3] * power;

    prog_morph.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, shape_texture1);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, shape_texture2);

    glUniform1i(pm_f_shape1, 0);
    glUniform1i(pm_f_shape2, 1);
    glUniform4f(pm_f_color, color.r, color.g, color.b, color.a);
    glUniform1f(pm_f_power, actual_power);
    glUniform1f(pm_f_progress, progress);
    glUniformMatrix4fv(pm_v_mvp, 1, GL_FALSE, &mvp[0][0]);
    
    glEnableVertexAttribArray(pm_v_pos);
    float vertices[] = {
        -1.0, 1.0, 1.0, 1.0, 1.0, -1.0,
        -1.0, 1.0, -1.0, -1.0, 1.0, -1.0,
    };
    glVertexAttribPointer(pm_v_pos, 2, GL_FLOAT, GL_FALSE, 0, vertices);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(pm_v_pos);
    prog_morph.unuse();
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Shape::Renderer::shape_texture(const Shape &shape, GLuint &texture) const noexcept{
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, shape.get_width(), shape.get_height(), 0, GL_RED, GL_FLOAT, shape.fragments.data());
    glBindTexture(GL_TEXTURE_2D, 0);
}
