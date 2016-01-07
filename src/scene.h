#pragma once

#include "tiny_obj_loader.h"

#include <GL/glew.h>
#include <GL/gl.h>

#include <string>
#include <vector>


class Scene {
    typedef struct {
        unsigned int indexCount;
        unsigned int indexOffset;
    } Mesh;

    enum Buffers {
        vertex, index, AMOUNT
    };

    public:
        bool Load_obj_file(std::string filename);

        void Draw();

    private:
        void gen_buffers();

        std::vector<Mesh> meshes;
        // put static data into a single continuous buffer
        std::vector<GLfloat> data;
        std::vector<GLuint> indices;

        GLuint VAO, *buffers;
};
