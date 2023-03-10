#pragma once

#include <array>
#include <vector>
#include <ios>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glutil/Program.hpp"

class Shape{
public:
    class Renderer;

    Shape(std::size_t width, std::size_t height) noexcept;
    Shape(FILE *stream, bool magic = true);
    Shape(std::vector<uint8_t> data, bool magic = true);
    Shape(const char *file);
    virtual ~Shape() noexcept = default;

    void write_to_stream(FILE *stream, bool write_magic = true) const;
    void write_to_file(const char *file) const;

    //uses cpu
    void draw_circle(glm::vec2 circle_pos, float cr) noexcept;

    std::size_t get_width() const noexcept;
    std::size_t get_height() const noexcept;
protected:
    std::vector<float> fragments;
private:
    void init_from_stream(FILE *stream);
    void init_from_stream_without_magic(FILE *stream);

    std::size_t width;
    std::size_t height;
};

class Shape::Renderer{
public:
    inline static const glm::mat4 IDENTITY = glm::identity<glm::mat4>();

    Renderer() noexcept;
    virtual ~Renderer() noexcept;
    
    //logic_error if already is_init
    void init();

    //can be called to destroy in spcific place (for better managing GlContext)
    void uninit();

    void render(GLuint shape_texture, const glm::vec4 &color, float power, const glm::mat4 &mvp, const glm::mat4 &tex_mvp) const noexcept;
    void render(GLuint shape_texture, const glm::vec4 &color, float power, const glm::mat4 &mvp) const noexcept;
    void render(GLuint shape_texture, const glm::vec4 &color, float power) const noexcept;
    void render_morph(GLuint shape_texture1, GLuint shape_texture2, const glm::vec4 &color , float power, float progress, const glm::mat4 &mvp, const glm::mat4 &tex_mvp) const noexcept;
    void render_morph(GLuint shape_texture1, GLuint shape_texture2, const glm::vec4 &color , float power, float progress, const glm::mat4 &mvp) const noexcept;
    void render_morph(GLuint shape_texture1, GLuint shape_texture2, const glm::vec4 &color , float power, float progress) const noexcept;

    void shape_texture(const Shape &shape, GLuint &texture) const noexcept;
    bool is_init() const noexcept;
private:
    GlUtil::Program prog_render;
        GLint pr_v_pos;
        GLint pr_v_mvp;
        GLint pr_v_tex_mvp;
        GLint pr_f_color;
        GLint pr_f_power;
        GLint pr_f_shape;
    
    GlUtil::Program prog_morph;
        GLint pm_v_pos;
        GLint pm_v_mvp;
        GLint pm_v_tex_mvp;
        GLint pm_f_color;
        GLint pm_f_power;
        GLint pm_f_shape1;
        GLint pm_f_shape2;
        GLint pm_f_progress;

    bool rel_to_width;
    bool _is_init;
    Renderer(const Renderer &copy) noexcept = delete;
    Renderer &operator=(const Renderer &copy) noexcept = delete;
};
