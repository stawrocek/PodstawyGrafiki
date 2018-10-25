#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <utility>

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

    std::vector<std::pair<std::string, GLenum>> openGLInfoDict {
      std::make_pair("OpenGL renderer: ", GL_RENDERER)
    , std::make_pair("Vendor: ", GL_VENDOR)
    , std::make_pair("OpenGL version: ", GL_VERSION)
    , std::make_pair("GLSL version: ", GL_SHADING_LANGUAGE_VERSION)
    };
    auto infoString = getOpenGLInfoString(openGLInfoDict);
    std::cout << infoString << std::endl;

    ShaderProgram program{"./shader/triangle.vert.glsl", "./shader/triangle.frag.glsl"};
    GLuint pos2DLoc = 0;

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
    std::cout << program.id << "\n";
    while(!glfwWindowShouldClose(window))
    {
        /*  Insert OpenGL magic here! */
        glClear(GL_COLOR_BUFFER_BIT);
        program.use();
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

    // glDeleteProgram(shaderProgram);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}