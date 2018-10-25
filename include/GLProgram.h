#ifndef GL_PROGRAM_H
#define GL_PROGRAM_H
#include <fstream>
#include <sstream>
#include <string>

#include <GL/glew.h>

#include "./GLUtils.h"

struct ShaderProgram {
    GLuint id;

    ShaderProgram(
          const std::string& vertFilename
        , const std::string& fragFilename
    ){
        std::ifstream vertFile{vertFilename};
        std::ifstream fragFile{fragFilename};
        std::stringstream vertSStream, fragSStream;

        vertFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fragFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            if (vertFile.is_open() && fragFile.is_open())
            {
                vertSStream << vertFile.rdbuf();
                fragSStream << fragFile.rdbuf();
            }
            else
            {
                std::cerr << "SHADER::FILES::NOT_OPEN\n";
                return;
            }
        }
        catch (std::ifstream::failure e)
        {
			std::cerr << "ERROR::SHADER_PROGRAM::SHADER_FILES\n";
            return;
        }

        // stringstream to string temporary and char* string issue
        // https://stackoverflow.com/questions/10642253/stringstream-c-str-corruption-in-c
        const auto vertString = vertSStream.str();
        const auto fragString = fragSStream.str();
        const auto* vertSource = vertString.c_str();
        const auto* fragSource = fragString.c_str();
#ifdef DEBUG
        std::cout << "vertex:\n" << vertSource
            << "\nfragment:\n" << fragSource << std::endl;
#endif

        GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertShader, 1, &vertSource, 0);
        glCompileShader(vertShader);
        checkShaderCompileStatus(vertShader);

        GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragShader, 1, &fragSource, 0);
        glCompileShader(fragShader);
        checkShaderCompileStatus(fragShader);

        id = glCreateProgram();
        glAttachShader(id, vertShader);
        glAttachShader(id, fragShader);
        glLinkProgram(id);

        GLint isLinked;
        glGetProgramiv(id, GL_LINK_STATUS, &isLinked);
        if (isLinked != GL_TRUE)
        {
            GLsizei logLength = 0;
            GLchar msgBuffer[1024];
            glGetProgramInfoLog(id, 1024, &logLength, msgBuffer);
            std::cerr << "ERROR::PROGRAM::" << id << "::LINK\n"
                << msgBuffer << std::endl;
        }

        glDeleteShader(vertShader);
        glDeleteShader(fragShader);
    }
    ~ShaderProgram()
    {
        glDeleteProgram(id);
    }
    void use()
    {
        glUseProgram(id);
    }
};
#endif  // GL_PROGRAM_H