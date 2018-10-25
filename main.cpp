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
    /* Transformations! */

    // simple transformations with GLM
    auto v1 = glm::vec4(1.0f,1.0f,1.0f,1.0f);
    auto translation = glm::mat4(1.0f);
    translation = glm::translate(translation, glm::vec3(0.5f, 0.5f, 0.5f));
    std::cout << v1 << "\n";
    v1 = translation * v1;    
    std::cout << v1 << "\n";

    // applying transformations to geometry
    auto transMatLocation = glGetUniformLocation(program.id, "transform");
    program.use();  // !!!
    glUniformMatrix4fv(transMatLocation, 1, GL_FALSE, glm::value_ptr(translation));

    // compositing transformations
    auto rotation = glm::mat4(1.0f);
    auto scale = glm::mat4(1.0f);
    rotation = glm::rotate(rotation, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scale = glm::scale(scale, glm::vec3(0.5f, 0.5f, 1.0f));
    auto transformation = translation * scale * rotation;
    glUniformMatrix4fv(transMatLocation, 1, GL_FALSE, glm::value_ptr(transformation));

    // matrix order in OpenGL (column-major GLSL vs row-major C)
    const float cTranslation[4][4] = {
          {0.5f, 0.0f, 0.0f, -0.4f}
        , {0.0f, 0.5f, 0.0f, -0.4f}
        , {0.0f ,0.0f, 0.5f,  0.0f}
        , {0.0f, 0.0f, 0.0f,  1.0f}
    };
    glUniformMatrix4fv(transMatLocation, 1, GL_TRUE, &cTranslation[0][0]);
    /* Transformations! */

    /* Projections! */    
    // local 2 world <==> model
    auto model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f,0.0f,0.0f));
    glUniformMatrix4fv(transMatLocation, 1, GL_FALSE, glm::value_ptr(model));

    /* Camera and lookAt! */
    glm::vec3 worldOrigin = glm::vec3(0.0f);
    glm::vec3 worldUp = glm::vec3(0.0f,1.0f,0.0f);
    glm::vec3 camPosition = glm::vec3(0.0f,1.0f,3.0f);
    auto camZ = glm::normalize(camPosition - worldOrigin);
    auto camX = glm::cross(worldUp, camZ);
    auto camY = glm::cross(camZ, camX);

    glm::mat4 camOrthoBasis = glm::mat4(
          glm::vec4(camX, 0.0f)
        , glm::vec4(camY, 0.0f)
        , glm::vec4(camZ, 0.0f)
        , glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
    );
    camOrthoBasis = glm::transpose(camOrthoBasis);

    auto camTranslation = glm::mat4(1.0f);
    camTranslation[3] = glm::vec4(-camPosition, 1.0f);

    auto world2Camera = camOrthoBasis * camTranslation;
    auto lookAt = glm::lookAt(camPosition, worldOrigin, worldUp);

    std::cout << world2Camera << "\n";
    std::cout << lookAt << "\n";
    /* Camera and lookAt! */

    // world 2 camera <==> view - this transformation is opposite to word origin to camera 
    auto view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f,0.0f,-3.0f));
    view = world2Camera;
    glUniformMatrix4fv(transMatLocation, 1, GL_FALSE, glm::value_ptr(view * model)); // what will happen now?

    // camera 2 clip space 2 NDC <==> projection
    glm::mat4 projection;
    if (true)
    {
        projection = glm::perspective(
            glm::radians(45.0f)
            , (float)width/(float)height
            , 0.1f
            , 100.0f
        );
    }
    else
    {
        // careful with size!!!
        // https://www.opengl.org/discussion_boards/showthread.php/176144-glm-ortho-problem-SOLVED
        projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    }
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
        rotation = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime() ,glm::vec3(0.0f,0.0f,1.0f));
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