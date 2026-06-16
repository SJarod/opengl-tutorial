#pragma once

#include <iostream>

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "vertex.hpp"
#include "wsi.hpp"

namespace RHI
{

inline void load_symbols()
{
    if (!gladLoadGL((GLADloadfunc)WSI::get_proc_addr()))
        std::cerr << "Failed to load OpenGL functions" << std::endl;
}

namespace Memory
{
namespace Buffer
{
template <typename TType> inline GLuint create_buffer(std::vector<TType> data, GLenum usageFlags)
{
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(usageFlags, buffer);
    glBufferData(usageFlags, data.size() * sizeof(TType), data.data(), GL_STATIC_DRAW);
    glBindBuffer(usageFlags, 0);
    return buffer;
}
inline void destroy_buffer(GLuint *buffer)
{
    glDeleteBuffers(1, buffer);
}

inline GLuint create_vertex_array(GLuint vbo, const GLuint *ebo)
{
    GLuint array;
    glGenVertexArrays(1, &array);
    glBindVertexArray(array);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    if (ebo)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ebo);

    // strides
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FLOAT, sizeof(Vertex), (void *)offsetof(Vertex, position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FLOAT, sizeof(Vertex), (void *)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FLOAT, sizeof(Vertex), (void *)offsetof(Vertex, uv));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    if (ebo)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
    return array;
}
inline void destroy_vertex_array(GLuint *array)
{
    glDeleteVertexArrays(1, array);
}
} // namespace Buffer

namespace Image
{
inline GLuint create_texture(int width, int height, std::vector<unsigned char> data, bool bGenerateMipMap = true)
{
    GLuint texture;

    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
    if (bGenerateMipMap)
        glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}
inline void destroy_texture(GLuint *texture)
{
    glDeleteTextures(1, texture);
}
} // namespace Image
} // namespace Memory

namespace Shader
{
inline GLuint create_shader_program(const char *vsSource, const char *fsSource)
{
    GLuint program;

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vsSource, NULL);
    glCompileShader(vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fsSource, NULL);
    glCompileShader(fs);

    int success[2];
    char infoLog[2][512];
    glGetShaderiv(vs, GL_COMPILE_STATUS, &success[0]);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &success[1]);

    if (!success[0] || !success[1])
    {
        glGetShaderInfoLog(vs, 512, NULL, infoLog[0]);
        std::cout << infoLog[0] << std::endl;
        glGetShaderInfoLog(fs, 512, NULL, infoLog[1]);
        std::cout << infoLog[1] << std::endl;

        std::cerr << "Failed to compile shaders" << std::endl;
        return -1;
    }

    program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}
inline void destroy_shader_program(GLuint program)
{
    glDeleteProgram(program);
}

inline void set_uniform_matrix4(GLuint program, const char *uniformName, glm::mat4 matrix)
{
    GLuint location = glGetUniformLocation(program, uniformName);
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}
} // namespace Shader

namespace Render
{
inline void draw_object(GLuint program, GLuint vao, int vertexCount, bool bEnableDepthTest = true)
{
    glUseProgram(program);

    if (bEnableDepthTest)
        glEnable(GL_DEPTH_TEST);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}

inline void draw_element_object(GLuint program, GLuint vao, GLuint ebo, int indexCount, bool bEnableDepthTest = true)
{
    glUseProgram(program);

    if (bEnableDepthTest)
        glEnable(GL_DEPTH_TEST);

    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

inline void bind_texture(GLuint texture)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
}
} // namespace Render
} // namespace RHI