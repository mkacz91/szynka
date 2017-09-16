#ifndef SZYNKA_OPENGL_H
#define SZYNKA_OPENGL_H

#include "prefix.h"
#include <glad.h>       // Including GLAD before GLFW suppresses system OpenGL header that would
#include <GLFW/glfw3.h> // otherwise be included by the latter.
#include <vector>
#include <jogurt_math.h>

#include "int_range.h"

namespace szynka {

template <typename T>
int byte_size(const std::vector<T>& xs) { return xs.size() * sizeof(T); }

void inline glBufferData(GLenum target, int size, void const* data, GLenum usage = GL_STATIC_DRAW)
{
    ::glBufferData(target, size, data, usage);
}

template <typename T> inline
void glBufferData(GLenum target, const std::vector<T>& data, GLenum usage = GL_STATIC_DRAW)
{
    ::glBufferData(target, byte_size(data), data.data(), usage);
}

inline void glDrawArrays(GLenum target, GLint first, GLint count)
{
    ::glDrawArrays(target, first, count);
}

inline void glDrawArrays(GLenum target, const int_range& vertex_range)
{
    ::glDrawArrays(target, vertex_range.first, vertex_range.count);
}

inline void glUniformMatrix3fv(GLint location, const jogurt::mat3f& m)
{
    ::glUniformMatrix3fv(location, 1, GL_FALSE, reinterpret_cast<const float*>(&m));
}

inline void glUniform2f(GLint location, float x, float y)
{
    ::glUniform2f(location, x, y);
}

inline void glUniform2f(GLint location, const jogurt::vec2f& v)
{
    ::glUniform2f(location, v.x, v.y);
}

namespace gl {

extern bool lost;

class exception : public std::exception
{
public:
    exception(const string& what) : m_what(what) { }
    virtual const char* what() const noexcept override { return m_what.c_str(); }
private:
    string m_what;
};

#define gl_if_error(call) \
    glGetError(); \
    call; \
    for (GLenum error = glGetError(); error != GL_NO_ERROR; error = GL_NO_ERROR)

#define gl_error_guard(call) \
    gl_if_error (call) \
        throw ::szynka::gl::exception(::szynka::gl::constant_string(error) + " during " + #call);

inline void create_buffer(GLuint* buffer)
{
    assert(lost || *buffer == 0);
    glGenBuffers(1, buffer);
}

template <typename T>
void create_buffer(GLuint* buffer, const std::vector<T>& data, GLenum usage = GL_STATIC_DRAW)
{
    create_buffer(buffer);
    glBindBuffer(GL_ARRAY_BUFFER, *buffer);
    glBufferData(GL_ARRAY_BUFFER, data, usage);
}

inline void delete_buffer(GLuint* buffer)
{
    assert(!lost);
    glDeleteBuffers(1, buffer);
    *buffer = 0;
}

inline void delete_program(GLuint* program)
{
    assert(!lost);
    glDeleteProgram(*program);
    *program = 0;
}

void load_vertex_shader(GLuint* shader, const string& name);

void load_fragment_shader(GLuint* shader, const string& name);

inline void delete_shader(GLuint* shader)
{
    assert(!lost);
    glDeleteShader(*shader);
    *shader = 0;
}

string shader_name(GLuint shader);

void link_program(GLuint* program, GLuint vertex_shader, GLuint fragment_shader);

void link_program(
    GLuint* program,
    const string& vertex_shader_name, const string& fragment_shader_name
);

GLint get_uniform_location(GLuint program, const char* name);

GLint get_attrib_location(GLuint program, const char* name);

string constant_string(GLenum);

} // namespace gl
} // namespace szynka

#endif // SZYNKA_OPENGL_H
