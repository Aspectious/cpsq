#include "include/glad.h"
#include "include/glfw3.h"
#include <glm/glm.hpp>


#include <iostream>

#pragma region codeistoleonline

static const char* vertex_shader_source = R"glsl(
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
)glsl";

#pragma endregion codeistoleonline


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



    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    //glViewport(0, 0, width, height);

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertexShader);

    // 2. Check for compilation errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "poo";
    }

    // 3. Create a shader program and attach the shader
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);

    // 4. (Later you'll also attach a fragment shader here)

    // 5. Link the program
    glLinkProgram(shaderProgram);

    // 6. Use the shader program in your render loop
    ;
#pragma endregion othercodeistole




    // Render Loop
    while (!glfwWindowShouldClose(window)) {

        glUseProgram(shaderProgram);

        glfwSwapBuffers(window); // Swaps front and back buffers?
        glfwPollEvents(); // Polls events for things like keyboard/mouse input
    }

    // Clean up and die
    glfwTerminate();

    return 0;
}