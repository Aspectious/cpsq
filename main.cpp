#include "include/glad.h"
#include "include/glfw3.h"
#include <glm/glm.hpp>


#include <iostream>

#pragma region codeistoleonline

static const char* vertex_shader_1 = R"glsl(
#version 330 core
layout (location = 0) in vec3 aPos;

void main() {
  gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
)glsl";
static const char* vertex_shader_2 = R"glsl(
#version 330 core
out vec4 FragColor;

void main() {
  FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}
)glsl";

#pragma endregion codeistoleonline

unsigned int compileShader(const char* shaderpointer) {
    // 2. Create a shader object
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // 3. Attach the shader source
    glShaderSource(vertexShader, 1, &shaderpointer, NULL);

    // 4. Compile the shader
    glCompileShader(vertexShader);

    // 5. Check for compilation errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    return vertexShader;
}

int main() {

    // Lets get started
    glfwInit();

    // Something for backwards compatibility
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Creates a window and context
    GLFWwindow* window = glfwCreateWindow(800, 600, "2D Renderer", NULL, NULL);

    // Makes the context "current"
    glfwMakeContextCurrent(window);



#pragma region othercodeistole

    gladLoadGL();

    // 1. Define your shader source code
    const char* vertexShaderSource = R"glsl(
#version 330 core
layout (location = 0) in vec3 aPos;

void main() {
    gl_Position = vec4(aPos, 1.0);
}
)glsl";


    unsigned int prog1 = compileShader(vertex_shader_1);
    unsigned int prog2 = compileShader(vertex_shader_2);


    // 6. Use the shader program in your render loop
    ;
#pragma endregion othercodeistole




    // Render Loop
    while (!glfwWindowShouldClose(window)) {

        glUseProgram(prog1);
        glUseProgram(prog2);

        float vertices[] = {
            -0.5f, -0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
             0.0f,  0.5f, 0.0f
          };

        unsigned int VBO, VAO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Draw triangle
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window); // Swaps front and back buffers?
        glfwPollEvents(); // Polls events for things like keyboard/mouse input
    }

    // Clean up and die
    glfwTerminate();

    return 0;
}