# shapepp
continue implementing [shape](https://github.com/AnatolyRybchych/shape)


# demo
> initialization
```cpp
glEnable(GL_BLEND);
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

renderer.init();

glGenTextures(textures.size(), &textures[0]);
Shape s(128, 128);
s.draw_circle(glm::vec2(0.0, -0.5), 0.1);
s.draw_circle(glm::vec2(0.0, 0.5), 0.1);
s.draw_circle(glm::vec2(0.5, 0.0), 0.1);
s.draw_circle(glm::vec2(-0.5, 0.0), 0.1);

Shape s2(128, 128);
s2.draw_circle(glm::vec2(0.0, 0.0), 0.5);
renderer.shape_texture(s, textures[TEXTURE_SHAPE]);
renderer.shape_texture(s2, textures[TEXTURE_SHAPE2]);
```

> rendering
```cpp
glClearColor(0.0, 0.0, 0.0, 1.0);
glClear(GL_COLOR_BUFFER_BIT);

float time = SDL_GetTicks() * 0.001;
float progress = sinf(time) * 0.5 + 0.5;

glm::mat4 mvp = glm::identity<glm::mat4>();
glm::vec3 rotation(0.0, 0.0, 1.0);
mvp = glm::rotate(mvp, time, rotation);

glm::vec4 color(0.2, 0.4, 0.8, 1);

renderer.render_morph(textures[TEXTURE_SHAPE], textures[TEXTURE_SHAPE2], color, 0.5, progress, mvp);

SDL_GL_SwapWindow(window);
```
> thumbnail (only 15 fps)
![circles](/thumbnails/circles_15fps.gif)

> pixel image (if set texture filtering on GL_NEAREST and power paramter >= 2.0 in render method)
![pixel circles](/thumbnails/pixel_circles_15fps.gif)