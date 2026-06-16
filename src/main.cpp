#include <vector>

#include "vertex.hpp"

#include "opengl_minimal.hpp"

#include "triangle.frag.hpp"
#include "triangle.vert.hpp"

// TODO : remove glad and load opengl functions from dll

int main()
{
    WSI::init();

    int width = 1366, height = 768;

    GLFWwindow *window = WSI::create_window(width, height, "OpenGL Minimal");

    RHI::load_symbols();

    const std::vector<Vertex> vertices = {{{-0.5f, -0.5f, 0.f}, {1.f, 0.f, 0.f, 1.f}, {1.f, 0.f}},
                                          {{0.5f, -0.5f, 0.f}, {0.f, 1.f, 0.f, 1.f}, {0.f, 0.f}},
                                          {{0.5f, 0.5f, 0.f}, {0.f, 0.f, 1.f, 1.f}, {0.f, 1.f}},
                                          {{-0.5f, 0.5f, 0.f}, {1.f, 1.f, 1.f, 1.f}, {1.f, 1.f}},
                                          {{-0.5f, -0.5f, -0.5f}, {1.f, 0.f, 0.f, 1.f}, {1.f, 0.f}},
                                          {{0.5f, -0.5f, -0.5f}, {0.f, 1.f, 0.f, 1.f}, {0.f, 0.f}},
                                          {{0.5f, 0.5f, -0.5f}, {0.f, 0.f, 1.f, 1.f}, {0.f, 1.f}},
                                          {{-0.5f, 0.5f, -0.5f}, {1.f, 1.f, 1.f, 1.f}, {1.f, 1.f}}};
    GLuint vbo = RHI::Memory::Buffer::create_buffer<Vertex>(vertices, GL_ARRAY_BUFFER);

    const std::vector<unsigned int> indices = {0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4};
    GLuint ebo = RHI::Memory::Buffer::create_buffer<unsigned int>(indices, GL_ELEMENT_ARRAY_BUFFER);

    GLuint vao = RHI::Memory::Buffer::create_vertex_array(vbo, &ebo);

    GLuint program = RHI::Shader::create_shader_program(triangleVertSrc, triangleFragSrc);

    const std::vector<unsigned char> imagePixels = {255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 0, 255, 255};
    GLuint tex = RHI::Memory::Image::create_texture(2, 2, imagePixels);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // TODO : move these calls to encapsulated functions
        glViewport(0, 0, width, height);

        // TODO : move these calls to encapsulated functions
        glClearColor(0.2f, 0.2f, 0.2f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 model = glm::mat4(1.f);
        glm::mat4 view = glm::lookAt(glm::vec3(0.f, 1.f, 1.f), glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
        glm::mat4 proj = glm::perspective(glm::radians(45.f), width / (float)height, 0.1f, 1000.f);

        // TODO : compute mvp matrices before hand
        RHI::Shader::set_uniform_matrix4(program, "model", model);
        RHI::Shader::set_uniform_matrix4(program, "view", view);
        RHI::Shader::set_uniform_matrix4(program, "proj", proj);

        RHI::Render::bind_texture(tex);
        RHI::Render::draw_element_object(program, vao, ebo, indices.size());

        glfwSwapBuffers(window);
    }

    WSI::destroy_window(window);

    WSI::terminate();

    return EXIT_SUCCESS;
}
