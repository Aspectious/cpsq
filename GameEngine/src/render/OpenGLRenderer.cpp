#include "../../lib/include/glad.h"
#include "glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "cpsq.h"

namespace render {
    static unsigned int compileShader(const char *shaderpointer) {
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

    static unsigned int compileFShader(const char *shaderpointer) {
        // 2. Create a shader object
        unsigned int fragShader = glCreateShader(GL_FRAGMENT_SHADER);

        // 3. Attach the shader source
        glShaderSource(fragShader, 1, &shaderpointer, NULL);

        // 4. Compile the shader
        glCompileShader(fragShader);

        // 5. Check for compilation errors
        int success;
        char infoLog[512];
        glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragShader, 512, NULL, infoLog);
            std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
        return fragShader;
    }
    static char* loadShaderFromFile(std::string path) {

    }
    int create() {
        // Lets get started
        glfwInit();

        // Something for backwards compatibility
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // Creates a window and context
        GLFWwindow *window = glfwCreateWindow(800, 600, "2D Renderer", NULL, NULL);

        // Makes the context "current"
        glfwMakeContextCurrent(window);

        // Load OpenGL Methods with Glad
        gladLoadGL();

        char* vertex_shader_1 = loadShaderFromFile("assets/shaders/v_debugshape.glsl");
        char* fragment_shader_1 = loadShaderFromFile("assets/shaders/f_debugshape.glsl")

        unsigned int prog1 = compileShader(vertex_shader_1);
        unsigned int prog2 = compileFShader(fragment_shader_1);

        unsigned int shaderprog = glCreateProgram();
        glAttachShader(shaderprog, prog1);
        glAttachShader(shaderprog, prog2);
        glLinkProgram(shaderprog);

        int success;
        char infoLog[512];
        glGetShaderiv(shaderprog, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shaderprog, 512, NULL, infoLog);
            std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }


        float rotangle = 0.0f;
        // Render Loop
        while (!glfwWindowShouldClose(window)) {
            glUseProgram(shaderprog);

            //float vertices[] = {
            //    -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
            //     0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
            //     0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f
            //  };
            // Vertices (example for a unit cube centered at origin)
            float vertices[] = {
                // positions          // colors
                0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // top right
                0.5f, -0.5f, 0.0f, 1.0f, 0.5f, 0.0f, // bottom right
                -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, // bottom left
                -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // top left
                0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.5f, // top right
                0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, // bottom right
                -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, // bottom left
                -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f // top left
            };
            unsigned int indices[] = {
                0, 1, 3, // first triangle
                1, 2, 3, // second triangle

                4, 5, 7, // first triangle
                5, 6, 7, // second triangle

                0, 1, 5,
                5, 0, 4,

                2, 3, 6,
                3, 6, 1
            };

            unsigned int VBO, VAO, EBO;
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);

            glBindVertexArray(VAO);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices) * sizeof(unsigned int), indices, GL_STATIC_DRAW);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            // Position attribute
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) 0);
            glEnableVertexAttribArray(0);
            // Color attribute
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
            glEnableVertexAttribArray(1);

            if (rotangle > 360.0f) {
                rotangle -= 360.0f;
            } else {
                rotangle += 0.01f;
            }

            glm::mat4 model = glm::mat4(1.0f); // Initialize as identity matrix
            GLuint modelLoc = glGetUniformLocation(shaderprog, "transform");
            model = glm::rotate(model, glm::radians(rotangle), glm::vec3(1.0f, 1.0f, 1.0f));
            // Rotate 45 degrees around Z-axix
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            //model = glm::translate(model, glm::vec3(0.5f, -0.5f, 0.0f)); // Translate
            //model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f)); // Scale


            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            // Draw triangle

            glBindVertexArray(VAO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);
            //glDrawArrays(GL_TRIANGLES, 0, 3);

            glfwSwapBuffers(window); // Swaps front and back buffers?
            glfwPollEvents(); // Polls events for things like keyboard/mouse input
        }

        // Clean up and die
        glfwTerminate();

        return 0;
    }
}
