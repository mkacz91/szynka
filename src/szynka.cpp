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

    int framebuffer_size = 256;
    GLuint framebuffers[2];
    glGenFramebuffersEXT(2, framebuffers);
    GLuint framebuffer = framebuffers[0];
    glBindFramebufferEXT(GL_FRAMEBUFFER, framebuffer);

    GLuint renderbuffers[2];
    glGenRenderbuffersEXT(2, renderbuffers);

    GLuint color_attachment = renderbuffers[0];
    glBindRenderbufferEXT(GL_RENDERBUFFER, color_attachment);
    glRenderbufferStorageMultisampleEXT(
        GL_RENDERBUFFER, max_samples - 1, GL_RGBA8, framebuffer_size, framebuffer_size);
    glFramebufferRenderbufferEXT(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, color_attachment);

    GLuint stencil_attachment = renderbuffers[1];
    glBindRenderbufferEXT(GL_RENDERBUFFER, stencil_attachment);
    glRenderbufferStorageMultisampleEXT(
        GL_RENDERBUFFER, max_samples - 1, GL_STENCIL_INDEX1_EXT,
        framebuffer_size, framebuffer_size);
    glFramebufferRenderbufferEXT(
        GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, stencil_attachment);

    glViewport(0, 0, framebuffer_size, framebuffer_size);

    std::vector<vec2f> local_vertices =
    {
        { 0.10f, 0.10f },
        { 0.90f, 0.10f },
        { 0.10f, 0.90f },
        { 0.90f, 0.70f },
        { 0.10f, 0.10f }
    };
    int_range vertex_range(0, local_vertices.size());
    GLuint vertices = 0;
    gl::create_buffer(&vertices, local_vertices);

    std::vector<vec2f> local_viewport_vertices =
    {
        { 0.0f, 0.0f },
        { 1.0f, 0.0f },
        { 1.0f, 1.0f },
        { 0.0f, 1.0f }
    };
    int_range viewport_range(0, local_viewport_vertices.size());
    GLuint viewport_vertices = 0;
    gl::create_buffer(&viewport_vertices, local_viewport_vertices);

    GLuint fill_program = 0;
    gl::link_program(&fill_program, "fill_vx.glsl", "fill_fg.glsl");
    GLint fill_transform_uniform = gl::get_uniform_location(fill_program, "transform");
    GLint fill_position_attrib = gl::get_attrib_location(fill_program, "position");

    GLuint sdf_program = 0;
    gl::link_program(&sdf_program, "sdf_vx.glsl", "sdf_fg.glsl");
    GLint sdf_coverage_uniform = gl::get_uniform_location(sdf_program, "coverage");
    GLint sdf_transform0_uniform = gl::get_uniform_location(sdf_program, "transform0");
    GLint sdf_mask0_uniform = gl::get_uniform_location(sdf_program, "mask0");
    GLint sdf_transform1_uniform = gl::get_uniform_location(sdf_program, "transform1");
    GLint sdf_mask1_uniform = gl::get_uniform_location(sdf_program, "mask1");
    GLint sdf_position_attrib = gl::get_attrib_location(sdf_program, "position");

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearStencil(0);
    glClear(GL_STENCIL_BUFFER_BIT);

    glUseProgram(fill_program);
    glBindBuffer(GL_ARRAY_BUFFER, vertices);
    glEnableVertexAttribArray(fill_position_attrib);
    glVertexAttribPointer(fill_position_attrib, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnable(GL_STENCIL_TEST);

    glUniform4f(fill_transform_uniform, 1.0f, 0.0f, -1.0f, -1.0f);
    glStencilMask(0x01);

    glStencilOp(GL_INVERT, GL_INVERT, GL_INVERT);
    glStencilFunc(GL_ALWAYS, 0x00, 0x01);
    glColorMask(false, false, false, false);
    szynka::glDrawArrays(GL_TRIANGLE_FAN, vertex_range);

    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glStencilFunc(GL_EQUAL, 0x01, 0x01);
    glColorMask(true, false, false, false);
    szynka::glDrawArrays(GL_TRIANGLE_FAN, vertex_range);

    glUniform4f(fill_transform_uniform, 1.0f, 1.0f,
        (float)(-3) / (float)(2 * framebuffer_size),
        (float)(1 - 2 * framebuffer_size) / (2 * framebuffer_size));
    glStencilMask(0x02);
    glStencilOp(GL_INVERT, GL_INVERT, GL_INVERT);
    glStencilFunc(GL_ALWAYS, 0x00, 0x02);
    glColorMask(false, false, false, false);
    szynka::glDrawArrays(GL_TRIANGLE_FAN, vertex_range);

    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glStencilFunc(GL_EQUAL, 0x02, 0x02);
    glColorMask(false, true, false, false);
    szynka::glDrawArrays(GL_TRIANGLE_FAN, vertex_range);

    glDisableVertexAttribArray(fill_position_attrib);
    glDisable(GL_STENCIL_TEST);
    glColorMask(true, true, true, true);

    GLuint textures[2];
    glGenTextures(2, textures);

    GLuint coverage_texture = textures[0];
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, coverage_texture);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA, framebuffer_size, framebuffer_size, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GLuint transfer_framebuffer = framebuffers[1];
    glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER, transfer_framebuffer);
    glFramebufferTexture2DEXT(
        GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, coverage_texture, 0);
    glBlitFramebufferEXT(
        0, 0, framebuffer_size, framebuffer_size,
        0, 0, framebuffer_size, framebuffer_size,
        GL_COLOR_BUFFER_BIT, GL_NEAREST);

    int sdf_size = framebuffer_size / 2;
    GLuint sdf_texture = textures[1];
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, sdf_texture);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA, sdf_size, sdf_size, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2DEXT(
        GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sdf_texture, 0);
    glViewport(0, 0, sdf_size, sdf_size);

    glUseProgram(sdf_program);
    glUniform1i(sdf_coverage_uniform, 0);
    glUniform4f(sdf_transform0_uniform, 0.5f, 0.0f, 0.0f, 0.0f);
    glUniform4f(sdf_mask0_uniform, 1.0f, 0.0f, 0.0f, 0.0f);
    glUniform4f(sdf_transform1_uniform, 0.5f, 0.5f,
        (float)(framebuffer_size - 3) / (float)(2 * framebuffer_size),
        (float)(1) / (float)(2 * framebuffer_size));
    glUniform4f(sdf_mask1_uniform, 0.0f, 1.0f, 0.0f, 0.0f);
    glBindBuffer(GL_ARRAY_BUFFER, viewport_vertices);
    glEnableVertexAttribArray(sdf_position_attrib);
    glVertexAttribPointer(sdf_position_attrib, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    glClearColor(0.2f, 0.5f, 0.9f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    szynka::glDrawArrays(GL_TRIANGLE_FAN, viewport_range);

    glDisableVertexAttribArray(sdf_position_attrib);

    glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER, 0);
    glBindFramebufferEXT(GL_READ_FRAMEBUFFER, transfer_framebuffer);
    glViewport(0, 0, window_width, window_height);
    glClearColor(0.8f, 0.3f, 0.6f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBlitFramebufferEXT(
        0, 0, sdf_size, sdf_size,
        10, 10, 10 + sdf_size, 10 + sdf_size,
        GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glfwSwapBuffers(window);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }

    glDeleteTextures(2, textures);
    gl::delete_buffer(&viewport_vertices);
    gl::delete_buffer(&vertices);
    gl::delete_program(&sdf_program);
    gl::delete_program(&fill_program);
    glDeleteRenderbuffersEXT(2, renderbuffers);
    glDeleteFramebuffersEXT(2, framebuffers);

    glfwTerminate();
    return 0;
}
