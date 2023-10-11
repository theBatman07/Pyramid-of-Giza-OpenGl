#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <vector>
#include <cmath>
#include <GL/glut.h>
#include <string>

const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec2 aTexCoord;

    out vec2 TexCoord;
    out vec3 FragPos;
    out vec3 Normal;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main()
    {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
        TexCoord = aTexCoord;
        FragPos = vec3(model * vec4(aPos, 1.0));
        Normal = mat3(transpose(inverse(model))) * aPos;
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    in vec2 TexCoord;
    in vec3 FragPos;
    in vec3 Normal;

    out vec4 FragColor;

    uniform sampler2D textureSampler;
    uniform vec3 lightDirection;
    uniform vec3 lightColor;
    uniform vec3 objectColor;

    void main()
    {
        vec3 ambient = 0.5 * lightColor;
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(-lightDirection);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lightColor;
        vec3 result = (ambient + diffuse) * texture(textureSampler, TexCoord).rgb * objectColor;
        FragColor = vec4(result, 1.0);
    }
)";

float cameraSpeed = 0.05f;
glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPosition += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPosition -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void generateSphere(float radius, int sectors, int stacks, std::vector<float>& vertices, std::vector<unsigned int>& indices)
{
    float sectorStep = 2 * M_PI / sectors;
    float stackStep = M_PI / stacks;

    for (int i = 0; i <= stacks; ++i)
    {
        float stackAngle = M_PI / 2 - i * stackStep;
        float xy = radius * cosf(stackAngle);
        float z = radius * sinf(stackAngle);

        for (int j = 0; j <= sectors; ++j)
        {
            float sectorAngle = j * sectorStep;
            float x = xy * cosf(sectorAngle);
            float y = xy * sinf(sectorAngle);

            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
    }

    for (int i = 0; i < stacks; ++i)
    {
        int k1 = i * (sectors + 1);
        int k2 = k1 + sectors + 1;

        for (int j = 0; j < sectors; ++j, ++k1, ++k2)
        {
            if (i != 0)
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            if (i != stacks - 1)
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }
}
void print_text(int x, int y, const std::string& text) {
    glRasterPos2f(x, y);

    for (char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }
}
void display_welcome() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // Black background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 600);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glColor3f(0, 1, 0.5); // Spring green
    print_text(350, 600, "CMR INSTITUTE OF TECHNOLOGY");

    glColor3f(0.56, 0.56, 0.74); // Light steel blue
    print_text(250, 550, "DEPARTMENT OF COMPUTER SCIENCE AND ENGINEERING");

    glColor3f(0.8, 0.498, 0.196); // Gold
    print_text(200, 500, "18CSL67 - COMPUTER GRAPHICS WITH MINI PROJECT LABORATORY");

    glColor3f(0.196, 0.6, 0.8); // Sky blue
    print_text(10, 450, " ");

    glColor3f(0.196, 0.6, 0.8); // Sky blue
    print_text(350, 400, "PYRAMID OF GIZA");

    glColor3f(0.85, 0.85, 0.1); // Yellow
    print_text(100, 350, "Created By");
    glColor3f(0.45, 0.85, 0.57); // Light green
    print_text(100, 300, "GAURAV KHATER (1CR20CS065)");
    print_text(100, 250, "GOPAL NAIK (1CR20CS069)");

    glColor3f(0.85, 0.85, 0.1); // Yellow
    print_text(600, 350, "Guided By");
    glColor3f(0.45, 0.85, 0.57); // Light green
    print_text(500, 300, "Dr. Preethi Sheeba Hepsiba, Associate professor");
    print_text(500, 250, "Prof. Shivraj V B, Assistant Professor");

    glColor3f(0.85, 0.85, 0.1); // Yellow
    print_text(360, 100, "PRESS <<ENTER>> TO BEGIN");

    glutSwapBuffers();
}
void keyboard(unsigned char key, int x, int y) {
    if (key == 13) 
    { 
            glfwInit();
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

            GLFWwindow* window = glfwCreateWindow(800, 600, "Pyramid of Giza", nullptr, nullptr);
            if (window == nullptr)
            {
                std::cout << "Failed to create GLFW window" << std::endl;
                glfwTerminate();
                // return -1;
            }
            glfwMakeContextCurrent(window);

            glewExperimental = GL_TRUE;
            if (glewInit() != GLEW_OK)
            {
                std::cout << "Failed to initialize GLEW" << std::endl;
                // return -1;
            }

            // Define the pyramid's vertices and texture coordinates
            float pyramidVertices[] = {
                // Positions        // Texture coordinates
                0.0f,  1.0f,  0.0f,  0.5f, 1.0f,
                -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
                1.0f, -1.0f,  1.0f,  1.0f, 0.0f,
                0.0f,  1.0f,  0.0f,  0.5f, 1.0f,
                1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
                1.0f, -1.0f, -1.0f,  1.0f, 0.0f,
                0.0f,  1.0f,  0.0f,  0.5f, 1.0f,
                1.0f, -1.0f, -1.0f,  0.0f, 0.0f,
                -1.0f, -1.0f, -1.0f,  1.0f, 0.0f,
                0.0f,  1.0f,  0.0f,  0.5f, 1.0f,
                -1.0f, -1.0f, -1.0f,  0.0f, 0.0f,
                -1.0f, -1.0f,  1.0f,  1.0f, 0.0f,
            };

            GLuint pyramidVBO, pyramidVAO;
            glGenVertexArrays(1, &pyramidVAO);
            glGenBuffers(1, &pyramidVBO);

            glBindVertexArray(pyramidVAO);
            glBindBuffer(GL_ARRAY_BUFFER, pyramidVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);

            float sphereRadius = 0.5f;
            int sphereSectors = 36;
            int sphereStacks = 18;

            std::vector<float> sphereVertices;
            std::vector<unsigned int> sphereIndices;
            generateSphere(sphereRadius, sphereSectors, sphereStacks, sphereVertices, sphereIndices);

            GLuint sphereVBO, sphereEBO, sphereVAO;
            glGenVertexArrays(1, &sphereVAO);
            glGenBuffers(1, &sphereVBO);
            glGenBuffers(1, &sphereEBO);

            glBindVertexArray(sphereVAO);

            glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
            glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), sphereVertices.data(), GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(unsigned int), sphereIndices.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            // Load and create the texture
            GLuint texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);

            int width, height, numChannels;
            unsigned char* data = stbi_load("texture2.jpg", &width, &height, &numChannels, 0);
            if (data)
            {
                GLenum format;
                if (numChannels == 1)
                    format = GL_RED;
                else if (numChannels == 3)
                    format = GL_RGB;
                else if (numChannels == 4)
                    format = GL_RGBA;

                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);

                stbi_image_free(data);
            }
            else
            {
                std::cout << "Failed to load texture" << std::endl;
                stbi_image_free(data);
                // return -1;
            }

            GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
            glCompileShader(vertexShader);

            GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
            glCompileShader(fragmentShader);

            GLuint shaderProgram = glCreateProgram();
            glAttachShader(shaderProgram, vertexShader);
            glAttachShader(shaderProgram, fragmentShader);
            glLinkProgram(shaderProgram);

            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);

            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 view = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
            glm::mat4 projection = glm::perspective(glm::radians(120.0f), 800.0f / 600.0f, 0.1f, 100.0f);

            glUseProgram(shaderProgram);
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

            // Add a directional light source at the top right corner
            glm::vec3 lightDirection = glm::vec3(-0.5f, -0.5f, -0.5f);
            glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 0.8f); 

            glUniform3fv(glGetUniformLocation(shaderProgram, "lightDirection"), 1, glm::value_ptr(lightDirection));
            glUniform3fv(glGetUniformLocation(shaderProgram, "lightColor"), 1, glm::value_ptr(lightColor));
            glUniform3f(glGetUniformLocation(shaderProgram, "objectColor"), 1.0f, 1.0f, 1.0f); // Adjust the color of the object

            glEnable(GL_DEPTH_TEST);

            // Render loop
            while (!glfwWindowShouldClose(window))
            {
                processInput(window);

                int framebufferWidth, framebufferHeight;
                glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

                glViewport(0, 0, framebufferWidth, framebufferHeight);

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                // Clear the upper half with blue color
                glScissor(0, framebufferHeight / 2, framebufferWidth, framebufferHeight / 2);
                glEnable(GL_SCISSOR_TEST);
                glClearColor(0.529f, 0.808f, 0.922f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);
                glDisable(GL_SCISSOR_TEST);

                glClearColor(0.741f, 0.607f, 0.176f, 1.0f);
                glScissor(0, 0, framebufferWidth, framebufferHeight / 2);
                glEnable(GL_SCISSOR_TEST);
                glClear(GL_COLOR_BUFFER_BIT);
                glDisable(GL_SCISSOR_TEST);

                projection = glm::perspective(glm::radians(60.0f), static_cast<float>(framebufferWidth) / framebufferHeight, 0.1f, 100.0f);
                glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

                view = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
                glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

                float spacing = 1.1f;

                // Draw the first pyramid
                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(-2.0f - spacing, 0.0f, 0.0f)); // Adjust the translation values
                glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE,glm::value_ptr(model));
                glBindVertexArray(pyramidVAO);
                glDrawArrays(GL_TRIANGLES, 0, 12);

                // Draw the second pyramid
                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // Adjust the translation values
                glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
                glBindVertexArray(pyramidVAO);
                glDrawArrays(GL_TRIANGLES, 0, 12);

                // Draw the third pyramid
                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(2.0f + spacing, 0.0f, 0.0f)); // Adjust the translation values
                glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
                glBindVertexArray(pyramidVAO);
                glDrawArrays(GL_TRIANGLES, 0, 12);

                // Draw the sphere
                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(5.0f, 3.0f, 0.0f)); // Adjust the translation values to position the sphere above the pyramids

                // Set the color for the sphere
                glUniform3f(glGetUniformLocation(shaderProgram, "objectColor"), 1.0f, 0.64f, 0.0f); // Adjust the RGB values for the desired color

                glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
                glBindVertexArray(sphereVAO);
                glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);

                // Restore the original object color for the pyramids
                glUniform3f(glGetUniformLocation(shaderProgram, "objectColor"), 1.0f, 1.0f, 1.0f); // Reset the color to white


                glfwSwapBuffers(window);
                glfwPollEvents();
            }

            // Cleanup
            glDeleteVertexArrays(1, &pyramidVAO);
            glDeleteBuffers(1, &pyramidVBO);
            glDeleteVertexArrays(1, &sphereVAO);
            glDeleteBuffers(1, &sphereVBO);
            glDeleteTextures(1, &texture);
            glDeleteProgram(shaderProgram);

            glfwTerminate();
    }
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutDisplayFunc(display_welcome);
    glutKeyboardFunc(keyboard);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Welcome Window");

    glutDisplayFunc(display_welcome);
    glutKeyboardFunc(keyboard);

    glutMainLoop();

    return 0;

    return 0;
}
//  g++ pyramid.cpp -o pyramid -lglfw -lGLEW -lGL -ldl -lGLU -lglut