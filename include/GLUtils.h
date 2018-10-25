#ifndef GL_UTILS_H
#define GL_UTILS_H
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <utility>

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

std::string getOpenGLInfoString(
      const std::vector<std::pair<std::string, GLenum>>& dictionary
    , const std::string& sep = "\n"
    ){
    std::stringstream OpenGLInfoSS {};
    for (const auto& entry: dictionary)
    {
        auto infoString = glGetString(entry.second);
        OpenGLInfoSS << entry.first << infoString << sep;
    }
    return OpenGLInfoSS.str();
}

void checkShaderCompileStatus(const GLuint shaderId)
{
    GLint ifCompiled;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &ifCompiled);
    if(ifCompiled != GL_TRUE)
    {
        GLsizei logLength = 0;
        GLchar msgBuffer[1024];
        glGetShaderInfoLog(shaderId, 1024, &logLength, msgBuffer);
        std::cerr << "ERROR::SHADER::" << shaderId << "::COMPILE\n"
            << msgBuffer << std::endl;
    }
}

std::ostream& operator<<(std::ostream& os, const glm::vec3& v)
{
    return os << v.x << " " << v.y << " " << v.z;
}
#endif  // GL_UTILS_H