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

#define CULLING_ON 1

struct Vertex3DN {
    glm::vec3 pos;
    glm::vec3 normal;
};

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

    ShaderProgram program{"./shader/blinnphong.vert.glsl", "./shader/blinnphong.frag.glsl"};
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    auto normalUp = glm::vec3(0.0f, 1.0f, 0.0f);
    std::vector<Vertex3DN> vertices = {
          Vertex3DN{glm::vec3(1.0f, 0.0f, -1.0f), normalUp}
        , Vertex3DN{glm::vec3(-1.0f, 0.0f, -1.0f), normalUp}
        , Vertex3DN{glm::vec3(-1.0f, 0.0f, 1.0f), normalUp}
        , Vertex3DN{glm::vec3(1.0f, 0.0f, 1.0f), normalUp}
    };
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex3DN)*vertices.size(), vertices.data(), GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3DN), (void*)0);
    glEnableVertexAttribArray(0);
    // normal vectors
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3DN), (void*)(sizeof(float)*3));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    /* Projections! */    
    // local 2 world <==> model
    auto model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(1.5f));


    // world 2 camera <==> view - this transformation is opposite to word origin to camera 
    auto worldOrigin = glm::vec3(0.0f);
    auto worldUp = glm::vec3(0.0f,1.0f,0.0f);
    auto camPosition = glm::vec3(0.0f,3.0f,5.0f);
    auto view = glm::lookAt(camPosition, worldOrigin, worldUp);

    // N - normal transform matrix !!!
    auto N = glm::transpose(glm::inverse(view * model));

    // camera 2 clip space 2 NDC <==> projection
    glm::mat4 projection = glm::perspective(
          glm::radians(45.0f)
        , (float)width/(float)height
        , 0.1f
        , 100.0f
    );

    // sending matrices
    auto mvMatLocation = glGetUniformLocation(program.id, "vm");
    auto viewMatLocation = glGetUniformLocation(program.id, "view");
    auto projectionMatLocation = glGetUniformLocation(program.id, "projection");
    auto NMatLocation = glGetUniformLocation(program.id, "N");

    program.use();  // !!!
    glUniformMatrix4fv(mvMatLocation, 1, GL_FALSE, glm::value_ptr(view * model));
    glUniformMatrix4fv(viewMatLocation, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionMatLocation, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(NMatLocation, 1, GL_FALSE, glm::value_ptr(N));
    /* Projections! */    

    /* Shading! */
    auto lightPosition = glm::vec3(0.0f, 3.0f, -5.0f);  // light world position
    auto lightIntensity = glm::vec3(0.6f);  // light intensity/color

    auto lightPosLoc = glGetUniformLocation(program.id, "light.position");
    glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPosition));

    auto lightIntensLoc = glGetUniformLocation(program.id, "light.intensity");
    glUniform3fv(lightIntensLoc, 1, glm::value_ptr(lightIntensity));

    auto Ia = 0.1f; // ambient intesity
    glm::vec3 ka, kd; // ambient and diffuse color
    ka = kd = glm::vec3(1.0f, 1.0f, 0.5f);
    auto ks = glm::vec3(1.0f); // specular color
    auto phongExp = 64.0f;  // Phong exponent

    glUniform1f(glGetUniformLocation(program.id, "phongExp"), phongExp);
    glUniform1f(glGetUniformLocation(program.id, "Ia"), Ia);
    auto loadUniform3fv = [&](const std::string& uniformName, const glm::vec3& v){
        glUniform3fv(
              glGetUniformLocation(program.id, uniformName.c_str())
            , 1
            , glm::value_ptr(v)
        );
    };
    loadUniform3fv("ka", ka);
    loadUniform3fv("kd", kd);
    loadUniform3fv("ks", ks);
    /* Shading! */

    glClearColor(.3f, .3f, .3f, 1.0f);
    // glPolygonMode(GL_FRONT, GL_FILL);

    glEnable(GL_DEPTH_TEST);
#if CULLING_ON
	glEnable(GL_CULL_FACE);     // cull face
	glCullFace(GL_BACK);        // cull back face
	glFrontFace(GL_CCW);        // GL_CCW for counter clock-wise
#endif
    /*  Insert OpenGL magic here! */
    while(!glfwWindowShouldClose(window))
    {
        /*  Insert OpenGL magic here! */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        program.use();

        auto rotation = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime() ,glm::vec3(0.0f,1.0f,0.0f));
        glUniformMatrix4fv(mvMatLocation, 1, GL_FALSE, glm::value_ptr(view * (rotation * model)));

        N = glm::transpose(glm::inverse(view * model * rotation));
        glUniformMatrix4fv(NMatLocation, 1, GL_FALSE, glm::value_ptr(N));

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        /*  Insert OpenGL magic here! */
        glfwSwapBuffers(window);
        glfwPollEvents();  // gotta go fast!
        // glfwWaitEvents();  // save some cycles!
        if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE))
        {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
        if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_LEFT))
        {
            phongExp = glm::max(phongExp - 0.2f, 0.0f);
            glUniform1f(glGetUniformLocation(program.id, "phongExp"), phongExp);
            std::cout << "phongExp: " << phongExp << "\n";
        }
        if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_RIGHT))
        {
            phongExp = glm::min(phongExp + 0.2f, 256.0f);
            glUniform1f(glGetUniformLocation(program.id, "phongExp"), phongExp);
            std::cout << "phongExp: " << phongExp << "\n";
        }
    }

    // glDeleteProgram(shaderProgram);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}