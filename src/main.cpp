#include <iostream>
#include <fstream>
#include <array>

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include "glutil/Program.hpp"
#include "Shape.hpp"

enum Texture{
    TEXTURE_SHAPE,
    TEXTURE_SHAPE2,

    TEXTRES_COUNT,
};

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

        on_destruct();
    }
private:
    void on_init(){
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        renderer.init();
        
        glGenTextures(textures.size(), &textures[0]);
        Shape s("circle.shape");
        Shape s2("circle2.shape");

        renderer.shape_texture(s, textures[TEXTURE_SHAPE]);
        renderer.shape_texture(s2, textures[TEXTURE_SHAPE2]);
    }

    void on_destruct(){
        glDeleteTextures(textures.size(), &textures[0]);
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

        renderer.render_morph(textures[TEXTURE_SHAPE], textures[TEXTURE_SHAPE2], glm::vec4(1, 0, 0, 1), 0.5, sinf(SDL_GetTicks() * 0.005));

        SDL_GL_SwapWindow(window);
    }

    Shape::Renderer renderer;
    std::array<GLuint, Texture::TEXTRES_COUNT> textures;
    bool alive;
    SDL_Window *window;
};

int main(void){
    App app;

    app.run();
    return 0;
}
