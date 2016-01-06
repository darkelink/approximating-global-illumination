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

        // this seems silly
        void Set_uniformm4fv(const GLchar* uniform, glm::mat4 matrix);
        void Set_uniform1f(const GLchar* uniform, GLfloat value);
        void Set_uniform4f(const GLchar* uniform, glm::vec4 value);
        void Set_uniform1i(const GLchar* uniform, GLint value);

    private:
        std::vector<GLuint> ids;
        GLuint programid;
};
