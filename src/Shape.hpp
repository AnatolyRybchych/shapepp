#pragma once

#include <array>
#include <vector>
#include <ios>
#include "glutil/Program.hpp"

class Shape{
public:
    class Renderer;

    Shape(std::size_t width, std::size_t height) noexcept;
    Shape(FILE *stream);
    virtual ~Shape() noexcept = default;

    std::size_t get_width() const noexcept;
    std::size_t get_height() const noexcept;
protected:
    std::vector<float> fragments;
private:
    std::size_t width;
    std::size_t height;
};

class Shape::Renderer{
public:
    Renderer() noexcept;
    virtual ~Renderer() noexcept;
    
    //logic_error if already is_init
    void init();

    //can be called to destroy in spcific place (for better managing GlContext)
    void uninit();

    void render(GLuint shape_texture, const std::array<float, 4> color, float power) const noexcept;

    void shape_texture(const Shape &shape, GLuint &texture) const noexcept;
    bool is_init() const noexcept;
private:
    GlUtil::Program prog_render;
        GLint pr_v_pos;
        GLint pr_f_color;
        GLint pr_f_power;
        GLint pr_f_shape;

    bool rel_to_width;
    bool _is_init;
    Renderer(const Renderer &copy) noexcept = delete;
    Renderer &operator=(const Renderer &copy) noexcept = delete;
};
