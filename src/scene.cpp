#define TINYOBJLOADER_IMPLEMENTATION
#include "scene.h"

#include <iostream>

bool Scene::Load_obj_file(std::string filename) {
    std::string err;

    std::vector<tinyobj::shape_t> shapes;
    // TODO: materials
    std::vector<tinyobj::material_t> materials;

    bool ret = tinyobj::LoadObj(shapes, materials, err, filename.c_str(), "res/models/");
    if (!err.empty()) {
        std::cerr << err << std::endl;
    }

    int num = shapes.size();
    meshes.reserve(num);

    int dataOffset = 0;
    int indexOffset = 0;
    for (int i = 0; i < num; ++i) {
        meshes.push_back(Mesh {
            (unsigned int)shapes[i].mesh.positions.size(), // all vertex data is the same size
            (unsigned int)dataOffset,
            (unsigned int)shapes[i].mesh.indices.size(),
            (unsigned int)indexOffset
        });

        std::copy(std::begin(shapes[i].mesh.positions), std::end(shapes[i].mesh.positions),
                std::back_inserter(data));
        std::copy(std::begin(shapes[i].mesh.normals), std::end(shapes[i].mesh.normals),
                std::back_inserter(data));
        dataOffset += shapes[i].mesh.positions.size() + shapes[i].mesh.normals.size();

        std::copy(std::begin(shapes[i].mesh.indices), std::end(shapes[i].mesh.indices),
                std::back_inserter(indices));
        indexOffset += shapes[i].mesh.indices.size();
    }

    gen_buffers();

    return ret;
}

void Scene::gen_buffers() {
    int num = meshes.size();

    // use a vao for each mesh
    VAOs = new GLuint[num];
    glGenVertexArrays(num, VAOs);

    buffers = new GLuint[Buffers::AMOUNT];
    glGenBuffers(Buffers::AMOUNT, buffers);

    // upload all the data
    glBindBuffer(GL_ARRAY_BUFFER, buffers[Buffers::vertex]);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat),
            &data.front(), GL_STATIC_DRAW);

    // indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[Buffers::index]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint),
        &indices.front(), GL_STATIC_DRAW);

    for (int i = 0; i < num; ++i) {
        glBindVertexArray(VAOs[i]);

        // associate the vbos with the currently bounded vao
        glBindBuffer(GL_ARRAY_BUFFER, buffers[Buffers::vertex]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[Buffers::index]);

        // vertices
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0,
                (void *)(meshes[i].dataOffset * sizeof(GLfloat)));

        // normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0,
            (void*)((meshes[i].dataOffset + meshes[i].dataCount) * sizeof(GLfloat)));
    }
}

void Scene::Draw() {
    int count = meshes.size();
    for (int i = 0; i < count; ++i) {
        glBindVertexArray(VAOs[i]);
        glDrawElements(GL_TRIANGLES, meshes[i].indexCount, GL_UNSIGNED_INT,
                (void*)(meshes[i].indexOffset * sizeof(GLuint)));
    }
}

