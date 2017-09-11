#include <iostream>
#include <jogurt_math.h>
#include <vector>

#include "opengl.h"
#include "assets.h"

using namespace szynka;
using namespace jogurt;

int main(int argc, char** argv)
{
    GLFWwindow* window;
    vec2f x = zero2f();

    if (!glfwInit())
        return 1;

    window = glfwCreateWindow(800, 600, "Szynka", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    Assets::add_config("szynka_assets.config");
    std::vector<vec2f> local_vertices =
    {
        { 0, 0 },
        { 1, 0 },
        { -0.5f, 0.5f },
        { 1, 1 },
        { 0, 1 },
        { 0, 0 }
    };
    int_range vertex_range(0, local_vertices.size());
    GLuint vertices = 0;
    gl::create_buffer(&vertices, local_vertices);

    GLuint fill_program;
    gl::link_program(&fill_program, "fill_vx.glsl", "fill_fg.glsl");
    GLint fill_transform_uniform = gl::get_uniform_location(fill_program, "transform");
    GLint fill_position_attrib = gl::get_attrib_location(fill_program, "position");

    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(fill_program);
    szynka::glUniformMatrix3fv(fill_transform_uniform, eye3f().scale(0.5f));
    glEnableVertexAttribArray(fill_position_attrib);
    glVertexAttribPointer(fill_position_attrib, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    szynka::glDrawArrays(GL_TRIANGLE_FAN, vertex_range);

    glDisableVertexAttribArray(fill_position_attrib);

    glfwSwapBuffers(window);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }

    gl::delete_buffer(&vertices);
    gl::delete_program(&fill_program);

    glfwTerminate();
    return 0;
}
