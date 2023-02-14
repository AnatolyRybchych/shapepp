#include <iostream>
#include <SDL2/SDL.h>
#include <GL/glew.h>

#include "glutil/Program.hpp"

class App{
public:
    void run(){
        if(SDL_Init(SDL_INIT_VIDEO) < 0) exit(1);

        window = SDL_CreateWindow("", 0, 0, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
        if(!window) exit(2);

        SDL_GLContext gl_rc = SDL_GL_CreateContext(window);
        if(!gl_rc) exit(3);

        SDL_GL_MakeCurrent(window, gl_rc);
        if(glewInit() != GLEW_OK) exit(4);

        on_init();

        SDL_Event ev;
        alive = true;
        while (alive){
            if(SDL_PollEvent(&ev)){
                on_event(ev);
            }
            else{
                on_render();
            }
        }
    }
private:
    void on_init(){
        GlUtil::Shader vert = GlUtil::Shader::compile_new(
            GL_VERTEX_SHADER, R"GLSL(
                #version 110

                attribute vec4 v_pos;

                void main(){
                    gl_Position = v_pos;
                }
            )GLSL"
        );

        GlUtil::Shader frag = GlUtil::Shader::compile_new(
            GL_FRAGMENT_SHADER, R"GLSL(
                #version 110

                void main(){
                    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
                }
            )GLSL"
        );

        prog = GlUtil::Program::link_new(vert, frag);

        vert.delete_shader();
        frag.delete_shader();
    }

    void on_event(const SDL_Event &ev){
        switch (ev.type){
        case SDL_QUIT:
            alive = false;
            break;
        case SDL_WINDOWEVENT:
            on_winevent(ev.window);
            break;
        }
    }

    void on_winevent(const SDL_WindowEvent &ev){
        switch (ev.event){
        case SDL_WINDOWEVENT_RESIZED:
            glViewport(0, 0, ev.data1, ev.data2);
            break;
        }
    }

    void on_render(){
        glClear(GL_COLOR_BUFFER_BIT);

        prog.use();
        GLint v_pos = prog.attrib_location("v_pos");
        glEnableVertexAttribArray(v_pos);
        float vertices[] = {
            0.1, 0.8,
            -0.8, -0.7,
            0.8, -0.7,
        };

        glVertexAttribPointer(v_pos, 2, GL_FLOAT, GL_FALSE, 0, vertices);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glDisableVertexAttribArray(v_pos);
        prog.unuse();

        SDL_GL_SwapWindow(window);
    }

    
    GlUtil::Program prog;
    bool alive;
    SDL_Window *window;
};

int main(void){
    App app;

    app.run();
    return 0;
}
