#include <iostream>
#include <jogurt_math.h>
#include <vector>
#include <string>
#include <iomanip>

#include "opengl.h"
#include "assets.h"

using namespace szynka;
using namespace jogurt;
using std::string;

int load_map(string const& name, std::vector<GLubyte>& map_data);

int main(int argc, char** argv)
{
    GLFWwindow* window;

    if (!glfwInit())
        return 1;

    glfwWindowHint(GLFW_STENCIL_BITS, 8);
    glfwWindowHint(GLFW_SAMPLES, 16);

    int window_width = 800, window_height = 600;
    window = glfwCreateWindow(window_width, window_height, "Szynka", nullptr, nullptr);
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
    for (int i = 0; i < local_vertices.size(); ++i)
        local_vertices[i] = vec2f(0.5f, 0.5f) + (local_vertices[i] - vec2f(0.5f, 0.5f)) * 0.2f;
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
    GLint sdf_map_uniform = gl::get_uniform_location(sdf_program, "map");
    GLint sdf_map_uv_adjust = gl::get_uniform_location(sdf_program, "map_uv_adjust");
    GLint sdf_transform_n0_uniform = gl::get_uniform_location(sdf_program, "transform_n0");
    GLint sdf_transform_n1_uniform = gl::get_uniform_location(sdf_program, "transform_n1");
    GLint sdf_transform_r0_uniform = gl::get_uniform_location(sdf_program, "transform_r0");
    GLint sdf_transform_r1_uniform = gl::get_uniform_location(sdf_program, "transform_r1");
    GLint sdf_mask_n0_uniform = gl::get_uniform_location(sdf_program, "mask_n0");
    GLint sdf_mask_n1_uniform = gl::get_uniform_location(sdf_program, "mask_n1");
    GLint sdf_mask_r0_uniform = gl::get_uniform_location(sdf_program, "mask_r0");
    GLint sdf_mask_r1_uniform = gl::get_uniform_location(sdf_program, "mask_r1");
    GLint sdf_position_attrib = gl::get_attrib_location(sdf_program, "position");

    GLuint blit_program = 0;
    gl::link_program(&blit_program, "blit_vx.glsl", "blit_fg.glsl");
    GLint blit_rect_uniform = gl::get_uniform_location(blit_program, "rect");
    GLint blit_uv_adjust_uniform = gl::get_uniform_location(blit_program, "uv_adjust");
    GLint blit_texture_uniform = gl::get_uniform_location(blit_program, "texture");
    GLint blit_uv_attrib = gl::get_attrib_location(blit_program, "uv");

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearStencil(0);
    glClear(GL_STENCIL_BUFFER_BIT);

    glUseProgram(fill_program);
    glBindBuffer(GL_ARRAY_BUFFER, vertices);
    glEnableVertexAttribArray(fill_position_attrib);
    glVertexAttribPointer(fill_position_attrib, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnable(GL_STENCIL_TEST);

    glUniform4f(fill_transform_uniform, 0.5f, 0.0f, 0.0f, 0.0f);
    glStencilMask(0x01);

    glStencilOp(GL_INVERT, GL_INVERT, GL_INVERT);
    glStencilFunc(GL_ALWAYS, 0x00, 0x01);
    glColorMask(false, false, false, false);
    szynka::glDrawArrays(GL_TRIANGLE_FAN, vertex_range);

    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glStencilFunc(GL_EQUAL, 0x01, 0x01);
    glColorMask(true, false, false, false);
    szynka::glDrawArrays(GL_TRIANGLE_FAN, vertex_range);
    glStencilFunc(GL_ALWAYS, 0x00, 0x01);
    glColorMask(false, true, false, false);
    szynka::glDrawArrays(GL_LINE_STRIP, vertex_range);

    glUniform4f(fill_transform_uniform, 0.5f, 0.5f,
        (float)(framebuffer_size - 3) / (float)(2 * framebuffer_size),
        (float)(1) / (2 * framebuffer_size));
    glStencilMask(0x02);
    glStencilOp(GL_INVERT, GL_INVERT, GL_INVERT);
    glStencilFunc(GL_ALWAYS, 0x00, 0x02);
    glColorMask(false, false, false, false);
    szynka::glDrawArrays(GL_TRIANGLE_FAN, vertex_range);

    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glStencilFunc(GL_EQUAL, 0x02, 0x02);
    glColorMask(false, false, true, false);
    szynka::glDrawArrays(GL_TRIANGLE_FAN, vertex_range);
    glStencilFunc(GL_ALWAYS, 0x00, 0x02);
    glColorMask(false, false, false, true);
    glLineWidth(2);
    szynka::glDrawArrays(GL_LINE_STRIP, vertex_range);

    glDisableVertexAttribArray(fill_position_attrib);
    glDisable(GL_STENCIL_TEST);
    glColorMask(true, true, true, true);

    GLuint textures[3];
    glGenTextures(3, textures);

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

    auto map_in = Assets::open("map.txt", "generic");
    int map_size;
    map_in >> map_size;
    std::cout << "size: " << map_size << std::endl;
    std::vector<GLubyte> map_data(4 * map_size * map_size);
    std::cout << "\nFill channel:\n\n";
    for (int i = 0; i < map_size; ++i)
    {
        for (int j = 0; j < map_size; ++j)
        {
            int texel;
            map_in >> std::hex >> texel;
            map_data[4 * (i * map_size + j) + 0] = (GLubyte)texel;
            std::cout << std::hex << std::setw(2) << std::setfill('0') << texel << ' ';
        }
        std::cout << '\n';
    }
    std::cout << std::flush;
    std::cout << "Border channel:\n\n";
    for (int i = 0; i < map_size; ++i)
    {
        for (int j = 0; j < map_size; ++j)
        {
            int texel;
            map_in >> std::hex >> texel;
            map_data[4 * (i * map_size + j) + 1] = (GLubyte)texel;
            std::cout << std::hex << std::setw(2) << std::setfill('0') << texel << ' ';
        }
        std::cout << '\n';
    }
    std::cout << std::flush;

    GLuint map_n_texture = textures[1];
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, map_n_texture);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA, map_size, map_size, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, map_data.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int sdf_size = framebuffer_size / 2;
    GLuint sdf_texture = textures[2];
    glActiveTexture(GL_TEXTURE0 + 2);
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
    glUniform1i(sdf_map_uniform, 1);
    ::glUniform2f(sdf_map_uv_adjust, 0.5f / map_size, 1.0f - 1.0f / map_size);
    glUniform4f(sdf_transform_n0_uniform, 0.5f, 0.0f, 0.0f, 0.0f);
    glUniform4f(sdf_transform_n1_uniform, 0.5f, 0.0f, 0.0f, 0.0f);
    glUniform4f(sdf_transform_r0_uniform, 0.5f, 0.5f,
        (float)(framebuffer_size - 3) / (float)(2 * framebuffer_size),
        (float)(1) / (float)(2 * framebuffer_size));
    glUniform4f(sdf_transform_r1_uniform, 0.5f, 0.5f,
        (float)(framebuffer_size - 3) / (float)(2 * framebuffer_size),
        (float)(1) / (float)(2 * framebuffer_size));
    glUniform4f(sdf_mask_n0_uniform, 1.0f, 0.0f, 0.0f, 0.0f);
    glUniform4f(sdf_mask_n1_uniform, 0.0f, 1.0f, 0.0f, 0.0f);
    glUniform4f(sdf_mask_r0_uniform, 0.0f, 0.0f, 1.0f, 0.0f);
    glUniform4f(sdf_mask_r1_uniform, 0.0f, 0.0f, 0.0f, 1.0f);
    glBindBuffer(GL_ARRAY_BUFFER, viewport_vertices);
    glEnableVertexAttribArray(sdf_position_attrib);
    glVertexAttribPointer(sdf_position_attrib, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    szynka::glDrawArrays(GL_TRIANGLE_FAN, viewport_range);

    glDisableVertexAttribArray(sdf_position_attrib);

    glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER, 0);
    glViewport(0, 0, window_width, window_height);
    glClearColor(0.8f, 0.3f, 0.6f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(blit_program);
    glUniform1i(blit_texture_uniform, 2);
    ::glUniform2f(blit_uv_adjust_uniform, 0, 1);
    //::glUniform2f(blit_uv_adjust_uniform, 0.5f / map_size, 1.0f - 1.0f / map_size);
    if (window_width > window_height)
    {
        glUniform4f(blit_rect_uniform,
            -1.9 * window_height / window_width, -1.9, 1.9 * window_height / window_width, 1.9);
    }
    else
    {
        glUniform4f(blit_rect_uniform,
            -0.9, -0.9 * window_width / window_height, 0.9, 0.9 * window_width / window_height);
    }
    glBindBuffer(GL_ARRAY_BUFFER, viewport_vertices);
    glEnableVertexAttribArray(blit_uv_attrib);
    glVertexAttribPointer(blit_uv_attrib, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    szynka::glDrawArrays(GL_TRIANGLE_FAN, viewport_range);

    glDisableVertexAttribArray(blit_uv_attrib);

    glfwSwapBuffers(window);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }

    glDeleteTextures(3, textures);
    gl::delete_buffer(&viewport_vertices);
    gl::delete_buffer(&vertices);
    gl::delete_program(&sdf_program);
    gl::delete_program(&fill_program);
    glDeleteRenderbuffersEXT(2, renderbuffers);
    glDeleteFramebuffersEXT(2, framebuffers);

    glfwTerminate();
    return 0;
}

int load_map(string const& name, std::vector<GLubyte>& map_data)
{
    std::cout << "Loading map '" << name << "'\n";
    auto map_in = Assets::open(name + ".txt", "generic");
    int map_size;
    map_in >> map_size;
    std::cout << "size: " << map_size << std::endl;
    map_data.resize(map_size * map_size);
    for (int i = 0; i < map_size; ++i)
    {
        for (int j = 0; j < map_size; ++j)
        {
            int texel;
            map_in >> std::hex >> texel;
            map_data[i * map_size + j] = (GLubyte)texel;
            std::cout << std::hex << std::setw(2) << std::setfill('0') << texel << ' ';
        }
        std::cout << '\n';
    }
    std::cout << std::flush;
    return map_size;
}
