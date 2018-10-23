#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

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
    GLFWwindow *window = glfwCreateWindow(640, 480, "Hello window", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "GLFW3::WINDOW::CREATE::FAIL\n";
        glfwTerminate();
        return -1;
    }
    /*
     *  Insert OpenGL magic here!
     */
    while(!glfwWindowShouldClose(window))
    {
        /*
         *  Insert OpenGL magic here!
         */
        glfwSwapBuffers(window);
        glfwWaitEvents();  // save some cycles!
        if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}