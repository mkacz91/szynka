#include "opengl.h"

#include <unordered_map>

#include "assets.h"

namespace szynka {
namespace gl {

bool lost = false;

std::unordered_map<GLuint, string> shader_names;

GLuint load_shader(const string& name, GLenum shader_type)
{
    auto source = Assets::read_all_text(name, "shader");

    // Create shader object
    GLuint shader = glCreateShader(shader_type);
    if (shader == 0)
        throw exception("Unable to create new shader");

    // Upload source and compile
    char const* source_data = source.c_str();
    int source_size = source.length();
    glShaderSource(shader, 1, &source_data, &source_size);
    glCompileShader(shader);

    // TODO: Do this only in DEBUG

    int compile_status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);
    if (compile_status != GL_TRUE)
    {
        const int max_log_length = 255;
        char log[max_log_length + 1];
        glGetShaderInfoLog(shader, max_log_length, nullptr, (char*)&log);
        glDeleteShader(shader);
        throw exception("Shader " + squote(name) + ", compilation error: " + log);
    }

    shader_names[shader] = name;
    return shader;
}

void load_vertex_shader(GLuint* shader, const string& name)
{
    assert(lost || *shader == 0);
    *shader = load_shader(name, GL_VERTEX_SHADER);
}

void load_fragment_shader(GLuint* shader, const string& name)
{
    assert(lost || *shader == 0);
    *shader = load_shader(name, GL_FRAGMENT_SHADER);
}

string shader_name(GLuint shader)
{
    auto match = shader_names.find(shader);
    if (match != shader_names.end())
        return match->second;
    return to_string(shader);
}

void link_program(
    GLuint* program,
    const string& vertex_shader_name, const string& fragment_shader_name
) {
    GLuint vertex_shader = 0;
    GLuint fragment_shader = 0;
    try
    {
        load_vertex_shader(&vertex_shader, vertex_shader_name);
        load_fragment_shader(&fragment_shader, fragment_shader_name);
        link_program(program, vertex_shader, fragment_shader);
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
    }
    catch (...)
    {
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        throw;
    }
}

void link_program(GLuint* program, GLuint vertex_shader, GLuint fragment_shader)
{
    assert(lost || *program == 0);

    // Create program
    GLuint tmp_program = glCreateProgram();
    if (tmp_program == 0)
        throw exception("Unable to create new program");

    // Attach shaders
    gl_if_error (
        glAttachShader(tmp_program, vertex_shader);
        glAttachShader(tmp_program, fragment_shader);
    ) {
        glDeleteProgram(tmp_program);
        throw exception(
            "Unable to attach shaders " + squote(shader_name(vertex_shader)) + ", " +
            squote(shader_name(fragment_shader)) + ": "+ error_string(error)
        );
    }

    // Link program
    glLinkProgram(tmp_program);
    int link_status;
    glGetProgramiv(tmp_program, GL_LINK_STATUS, &link_status);
    if (link_status != GL_TRUE)
    {
        const int max_log_length = 255;
        char log[max_log_length + 1];
        glGetProgramInfoLog(tmp_program, max_log_length, nullptr, (char*)&log);
        glDeleteProgram(tmp_program);
        throw exception(
            "Unable to link shaders " + squote(shader_name(vertex_shader)) + ", " +
            squote(shader_name(fragment_shader)) + ": " + log
        );
    }

    *program = tmp_program;
}

GLint get_uniform_location(GLuint program, const char* name)
{
    gl_if_error (GLint location = glGetUniformLocation(program, name))
    {
        throw exception(
            error_string(error) + "during glGetUniformLocation(" +
            to_string(program) + ", " + quote(name) + ")"
        );
    }
    if (location < 0)
    {
        throw exception(
            "glGetUniformLocation(" + to_string(program) + ", " + quote(name) +
            ") returned " + to_string(location)
        );
    }
    return location;
}

GLint get_attrib_location(GLuint program, const char* name)
{
    gl_if_error (GLint location = glGetAttribLocation(program, name))
    {
        throw exception(
            error_string(error) + " during glGetAttribLocation(" +
            to_string(program) + ", " + quote(name) + ")"
        );
    }
    if (location < 0)
    {
        throw exception(
            "glGetAttribLocation(" + to_string(program) + ", " + quote(name) +
            ") returned " + to_string(location)
        );
    }
    return location;
}

string error_string(GLenum error)
{
#   define error_string_case(constant_name) case constant_name: return #constant_name
    switch (error)
    {
        error_string_case(GL_NO_ERROR);
        error_string_case(GL_INVALID_ENUM);
        error_string_case(GL_INVALID_VALUE);
        error_string_case(GL_INVALID_OPERATION);
        error_string_case(GL_OUT_OF_MEMORY);
        default: return "KLETCH_GL_UNKNOWN";
    }
}

} // namespace gl
} // namespace szynka
