#ifndef GL_UTILS_H
#define GL_UTILS_H
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <utility>

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

#endif  // GL_UTILS_H