#include "shader.h"

#include <iostream>
#include <glm/gtc/type_ptr.hpp>

bool Shader::Load(std::string source, GLenum type) {
    GLuint shaderid = glCreateShader(type);

    char const* source_p = source.c_str();
    glShaderSource(shaderid, 1, &source_p, NULL);
    glCompileShader(shaderid);

    // check error
    GLint success = 0;
    glGetShaderiv(shaderid, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        GLint errorLength = 0;
        glGetShaderiv(shaderid, GL_INFO_LOG_LENGTH, &errorLength);

        std::vector<GLchar> errorLog(errorLength);
        glGetShaderInfoLog(shaderid, errorLength, &errorLength, &errorLog[0]);

        std::cerr << std::endl << "Compile errors: " << std::endl;
        for (auto i: errorLog) {
            std::cerr << i;
        }
        std::cerr << std::endl;

        glDeleteShader(shaderid);
        return false;
    }

    ids.push_back(shaderid);
    return true;
}

bool Shader::Link() {
    programid = glCreateProgram();
    for (auto shaderid : ids) {
        glAttachShader(programid, shaderid);
    }
    glLinkProgram(programid);

    // check error
    GLint success = 0;
    glGetProgramiv(programid, GL_LINK_STATUS, (int *)&success);
    if (success == GL_FALSE) {
        GLint maxLength = 0;
        glGetProgramiv(programid, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(programid, maxLength, &maxLength, &infoLog[0]);

        std::cerr << "Linking errors: " << std::endl;
        for (auto i: infoLog) {
            std::cerr << i;
        }

        for (auto s: ids) {
            glDeleteShader(s);
        }

        glDeleteProgram(programid);
        return false;
    }

    // no longer need the individual shaders
    for (auto s: ids) {
        glDetachShader(programid, s);
        glDeleteShader(s);
    }
    ids.clear();

    Ready = true;
    return true;
}

void Shader::Use() {
    glUseProgram(programid);
}

void Shader::Delete() {
    glDeleteProgram(programid);
}

GLuint Shader::Get_ID() {
    return programid;
}
