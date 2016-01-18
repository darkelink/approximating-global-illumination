#pragma once

#include <glm/glm.hpp>

#include <string>
#include <vector>

#include <GL/glew.h>
#include <GL/gl.h>

class Shader {
    public:
        bool Ready = false;

        bool Load(std::string source, GLenum type);
        bool Link();

        void Use();
        void Delete();

        GLuint Get_ID();

    private:
        std::vector<GLuint> ids;
        GLuint programid;
};
