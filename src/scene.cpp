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

    int indexOffset = 0;
    int dataOffset = 0;
    for (int i = 0; i < num; ++i) {

        // avoid resizing the buffer too much
        data.reserve(data.size() + shapes[i].mesh.positions.size()*2);

        auto vertItr = std::begin(shapes[i].mesh.positions);
        auto normItr = std::begin(shapes[i].mesh.normals);

        auto end = std::end(shapes[i].mesh.positions);
        // interleave the data in the buffer (VVVNNNVVVNNN)
        while (vertItr != end) {
            data.push_back(*vertItr++);
            data.push_back(*vertItr++);
            data.push_back(*vertItr++);

            data.push_back(*normItr++);
            data.push_back(*normItr++);
            data.push_back(*normItr++);
        }

        // store mesh info in case we need to draw a single mesh at a time
        meshes.push_back(Mesh {
            (unsigned int)shapes[i].mesh.indices.size(),
            (unsigned int)indexOffset
        });

        for (auto index : shapes[i].mesh.indices) {
            // make sure indices point to the correct data
            indices.push_back(index + dataOffset);
        }

        dataOffset = data.size() / 6;
        indexOffset = indices.size();
    }

    gen_buffers();

    return ret;
}

void Scene::gen_buffers() {
    glGenVertexArrays(1, &VAO);

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

    glBindVertexArray(VAO);

    // associate the vbos with the currently bounded vao
    glBindBuffer(GL_ARRAY_BUFFER, buffers[Buffers::vertex]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[Buffers::index]);

    // vertices
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);

    // normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
        (void*)(3 * sizeof(GLfloat)));
}

void Scene::Draw() {
    //glBindVertexArray(VAO);

    // Draw everything at once
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);

    /* To draw a single mesh:
     * glDrawElements(GL_TRIANGLES, meshes[i].indexCount, GL_UNSIGNED_INT,
     *       (void*)(meshes[i].indexOffset * sizeof(GLuint)));
     */

}

