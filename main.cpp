#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "include/GLUtils.h"
#include "include/GLProgram.h"

int main(int argc, char* argv[])
{
    auto isInit = glfwInit();
    if (isInit != GLFW_TRUE)
    {
        std::cerr << "GLFW3::INIT::FAIL\n";
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE , GLFW_OPENGL_CORE_PROFILE);
#ifdef APPLE // <- those guys, ugh...
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    GLFWwindow *window = glfwCreateWindow(640, 480, "Hello triangle", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "GLFW3::WINDOW::CREATE::FAIL\n";
        glfwTerminate();
        return -1;
    }
    /*  Insert OpenGL magic here! */
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    auto isGLEW_OK = glewInit();
    if (isGLEW_OK != GLEW_OK)
    {
        std::cerr << "GLEW::INIT::FAIL\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    const char *vertexText[] = {
          "#version 410 core\n"
        , "in vec2 pos2D;\n"
        , "void main(){\n"
        , "gl_Position = vec4(pos2D, 0.0, 1.0);\n"
        , "}"
    };
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 5, vertexText, 0);
    glCompileShader(vertexShader);

    const char *fragmentText[] = {
          "#version 410 core\n"
        , "out vec4 outColor;\n"
        , "void main(){\n"
        , " outColor = vec4(1.0, 0.5, 1.0, 1.0);"
        , "}"
    };
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 5, fragmentText, 0);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glBindFragDataLocation(shaderProgram, 0, "outColor");
    glLinkProgram(shaderProgram);

    GLuint pos2DLoc = glGetAttribLocation(shaderProgram, "pos2D");

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    const float vertices[] = {
           0.0f,  0.5f
        , -0.5f, -0.5f
        ,  0.5f, -0.5f
    };
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*6, vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(pos2DLoc, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(pos2DLoc);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glClearColor(.3f, .3f, .3f, 1.0f);
    /*  Insert OpenGL magic here! */
    while(!glfwWindowShouldClose(window))
    {
        /*  Insert OpenGL magic here! */
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        /*  Insert OpenGL magic here! */
        glfwSwapBuffers(window);
        glfwPollEvents();  // gotta go fast!
        // glfwWaitEvents();  // save some cycles!
        if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}