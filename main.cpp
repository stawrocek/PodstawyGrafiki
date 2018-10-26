#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <utility>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
    const auto width = 640u;
    const auto height = 480u;
    GLFWwindow *window = glfwCreateWindow(width, height, "Hello triangle", NULL, NULL);
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
           0.5f,  0.5f
        , -0.5f,  0.5f
        , -0.5f, -0.5f
        ,  0.5f, -0.5f
    };
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*8, vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(pos2DLoc, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(pos2DLoc);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    /* Projections! */    
    // local 2 world <==> model
    auto model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f,0.0f,0.0f));

    // world 2 camera <==> view - this transformation is opposite to word origin to camera 
    auto worldOrigin = glm::vec3(0.0f);
    auto worldUp = glm::vec3(0.0f,1.0f,0.0f);
    auto camPosition = glm::vec3(0.0f,1.0f,3.0f);
    auto view = glm::lookAt(camPosition, worldOrigin, worldUp);

    // camera 2 clip space 2 NDC <==> projection
    glm::mat4 projection = glm::perspective(
          glm::radians(45.0f)
        , (float)width/(float)height
        , 0.1f
        , 100.0f
    );
    auto transMatLocation = glGetUniformLocation(program.id, "transform");
    program.use();  // !!!
    glUniformMatrix4fv(transMatLocation, 1, GL_FALSE, glm::value_ptr(projection * view * model));
    /* Projections! */    

    glClearColor(.3f, .3f, .3f, 1.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    /*  Insert OpenGL magic here! */
    while(!glfwWindowShouldClose(window))
    {
        /*  Insert OpenGL magic here! */
        glClear(GL_COLOR_BUFFER_BIT);
        program.use();
        auto rotation = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime() ,glm::vec3(0.0f,0.0f,1.0f));
        glUniformMatrix4fv(transMatLocation, 1, GL_FALSE, glm::value_ptr(projection * view * model * rotation));
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
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