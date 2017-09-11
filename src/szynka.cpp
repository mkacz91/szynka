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

    if (!glfwInit())
        return 1;

    glfwWindowHint(GLFW_STENCIL_BITS, 8);
    glfwWindowHint(GLFW_SAMPLES, 16);

    int window_width = 800, window_height = 600;
    window = glfwCreateWindow(800, 600, "Szynka", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    GLint max_samples;
    glGetIntegerv(GL_MAX_SAMPLES, &max_samples);
    std::cout << "max samples: " << max_samples << std::endl;

    Assets::add_config("szynka_assets.config");

    int framebuffer_width = 512, framebuffer_height = 512;
    GLuint framebuffer;
    glGenFramebuffersEXT(1, &framebuffer);
    glBindFramebufferEXT(GL_FRAMEBUFFER, framebuffer);

    GLuint renderbuffers[2];
    glGenRenderbuffersEXT(2, renderbuffers);

    GLuint color_attachment = renderbuffers[0];
    glBindRenderbufferEXT(GL_RENDERBUFFER, color_attachment);
    glRenderbufferStorageMultisampleEXT(
        GL_RENDERBUFFER, max_samples - 1, GL_RGBA8, framebuffer_width, framebuffer_height);
    glFramebufferRenderbufferEXT(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, color_attachment);

    GLuint stencil_attachment = renderbuffers[1];
    glBindRenderbufferEXT(GL_RENDERBUFFER, stencil_attachment);
    glRenderbufferStorageMultisampleEXT(
        GL_RENDERBUFFER, max_samples - 1, GL_STENCIL_INDEX1_EXT,
        framebuffer_width, framebuffer_height);
    glFramebufferRenderbufferEXT(
        GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, stencil_attachment);

    glViewport(0, 0, framebuffer_width, framebuffer_height);

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
    glClearStencil(0);
    glClear(GL_STENCIL_BUFFER_BIT);

    glUseProgram(fill_program);
    szynka::glUniformMatrix3fv(fill_transform_uniform, eye3f().scale(0.5f));
    glEnableVertexAttribArray(fill_position_attrib);
    glVertexAttribPointer(fill_position_attrib, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 0x00, 0x01);
    glStencilOp(GL_INVERT, GL_INVERT, GL_INVERT);
    glColorMask(false, false, false, false);

    szynka::glDrawArrays(GL_TRIANGLE_FAN, vertex_range);

    glStencilFunc(GL_EQUAL, 0x01, 0x01);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glColorMask(true, true, true, true);

    szynka::glDrawArrays(GL_TRIANGLE_FAN, vertex_range);

    glDisableVertexAttribArray(fill_position_attrib);

    glDisable(GL_STENCIL_TEST);

    glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER, 0);
    glViewport(0, 0, window_width, window_height);
    glClearColor(0.8f, 0.3f, 0.6f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBlitFramebufferEXT(
        0, 0, framebuffer_width, framebuffer_height,
        10, 10, 10 + framebuffer_width, 10 + framebuffer_height,
        GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glfwSwapBuffers(window);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }

    gl::delete_buffer(&vertices);
    gl::delete_program(&fill_program);
    glDeleteRenderbuffersEXT(2, renderbuffers);
    glDeleteFramebuffersEXT(1, &framebuffer);

    glfwTerminate();
    return 0;
}
