#define TINYOBJLOADER_IMPLEMENTATION
#include "scene.h"

#include "the_texture_manager.h"

#include <glm/glm.hpp>

#include <algorithm>
#include <iostream>
#include <map>

bool Scene::Load_obj_file(std::string filename) {
    std::string err;

    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    // assume all models are in res/models/ for now
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
        data.reserve(data.size() +
                shapes[i].mesh.positions.size()*2 + shapes[i].mesh.texcoords.size());

        auto vertItr = std::begin(shapes[i].mesh.positions);
        auto normItr = std::begin(shapes[i].mesh.normals);
        auto texItr  = std::begin(shapes[i].mesh.texcoords);

        float nextVertex;

        auto end = std::end(shapes[i].mesh.positions);
        // interleave vertex data in the buffer for rendering speed
        while (vertItr != end) {
            nextVertex = *vertItr++;
            size = glm::max(size, glm::abs(nextVertex));
            data.push_back(nextVertex);
            nextVertex = *vertItr++;
            size = glm::max(size, glm::abs(nextVertex));
            data.push_back(nextVertex);
            nextVertex = *vertItr++;
            size = glm::max(size, glm::abs(nextVertex));
            data.push_back(nextVertex);

            data.push_back(*normItr++);
            data.push_back(*normItr++);
            data.push_back(*normItr++);

            data.push_back(*texItr++);
            data.push_back(*texItr++);
        }

        meshes.push_back(Mesh {
                (unsigned int)shapes[i].mesh.indices.size(),
                (unsigned int)indexOffset,
                });


        auto matID = materials[shapes[i].mesh.material_ids[0]];
        std::string filename;
        // use texture filename as key for indexing materials
        if (matID.diffuse_texname.empty()) {
            filename = "";
        } else {
            filename = "res/models/" + matID.diffuse_texname;
            std::replace(filename.begin(), filename.end(), '\\', '/');
        }
        // mtl uses backslashes as directory separators for some reason

        uint64_t mat = TheTextureManager::Instance()->Create_from_file(filename);

        // group meshes with the same texture
        auto texmesh = texturedMeshes.find(mat);
        if (texmesh == texturedMeshes.end()) {
            texturedMeshes.insert(std::make_pair(mat, std::vector<Mesh*>{&meshes.back()}));
        } else {
            texmesh->second.push_back(&meshes.back());
        }

        for (auto index : shapes[i].mesh.indices) {
            // make sure indices point to the correct data
            indices.push_back(index + dataOffset);
        }

        dataOffset = data.size() / dataElements;
        indexOffset = indices.size();
    }
    glBindTexture(GL_TEXTURE_2D, 0);

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

    glEnableVertexAttribArray(VertexAttrib::location);
    glVertexAttribPointer(VertexAttrib::location, 3, GL_FLOAT, GL_FALSE,
            dataElements * sizeof(GLfloat), nullptr);

    glEnableVertexAttribArray(VertexAttrib::normal);
    glVertexAttribPointer(VertexAttrib::normal, 3, GL_FLOAT, GL_FALSE,
            dataElements * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

    glEnableVertexAttribArray(VertexAttrib::texcoord);
    glVertexAttribPointer(VertexAttrib::texcoord, 2, GL_FLOAT, GL_FALSE,
            dataElements * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
}

void Scene::Draw() {
    glBindVertexArray(VAO);

    for (auto texMesh : texturedMeshes) {
        glUniformHandleui64ARB(0, texMesh.first);
        for (auto mesh : texMesh.second) {
            glDrawElements(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT,
                    (void*)(mesh->indexOffset * sizeof(GLuint)));
        }
    }
}

void Scene::Draw_untextured() {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_BYTE, nullptr);
}

