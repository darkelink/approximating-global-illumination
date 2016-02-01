#define TINYOBJLOADER_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "scene.h"

#include "stb_image.h"

#include <algorithm>
#include <iostream>
#include <map>

static inline bool file_exists(std::string filename) {
    std::ifstream infile(filename.c_str());
    return infile.good();
}

bool Scene::Load_obj_file(std::string filename) {
    std::string err;

    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    bool ret = tinyobj::LoadObj(shapes, materials, err, filename.c_str(), "res/models/");
    if (!err.empty()) {
        std::cerr << err << std::endl;
    }

    std::map<std::string, uint64_t> textures;

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

        auto end = std::end(shapes[i].mesh.positions);
        // interleave vertex data in the buffer
        while (vertItr != end) {
            data.push_back(*vertItr++);
            data.push_back(*vertItr++);
            data.push_back(*vertItr++);

            data.push_back(*normItr++);
            data.push_back(*normItr++);
            data.push_back(*normItr++);

            data.push_back(*texItr++);
            data.push_back(*texItr++);
        }


        uint64_t mat;


        auto matID = materials[shapes[i].mesh.material_ids[0]];
        std::string filename = "res/models/" + matID.diffuse_texname;
        // mtl uses backslashes as directory separators for some reason
        std::replace(filename.begin(), filename.end(), '\\', '/');

        auto exists = textures.find(filename);

        if (exists == textures.end()) {
            if (matID.diffuse_texname.empty()) {
                mat = upload_texture("res/models/textures/missing.tga");
            } else if (!file_exists(filename)) {
                std::cerr << "Couldn't open texture: " << filename << std::endl;
                mat = upload_texture("res/models/textures/missing.tga");
            } else {
                mat = upload_texture(filename);
            }
            std::pair<std::string, uint64_t> newTex(filename, mat);
            textures.insert(newTex);
        } else {
            mat = exists->second;
        }

        // store mesh info in case we need to draw a single mesh at a time
        meshes.push_back(Mesh {
                (unsigned int)shapes[i].mesh.indices.size(),
                (unsigned int)indexOffset,
                });

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

    gen_buffers();

    return ret;
}

uint64_t Scene::upload_texture(std::string filename) {

    GLuint textureID;
    glGenTextures(1, &textureID);
    glActiveTexture(GL_TEXTURE0);

    unsigned char* image;
    int width, height, components;
    image = stbi_load(filename.c_str(), &width, &height, &components, 0);

    glBindTexture(GL_TEXTURE_2D, textureID);
    if (components == 3) {
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, width, height);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image);
    } else if (components == 4) {
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, image);
    } else {
        std::cerr << "bad texture format:" << filename << std::endl;
    }

    uint64_t handle = glGetTextureHandleARB(textureID);
    glMakeTextureHandleResidentARB(handle);

    stbi_image_free(image);

    return handle;
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
    //glBindVertexArray(VAO);

    // Draw everything at once
    // glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);

    for (auto texMesh : texturedMeshes) {
        glUniformHandleui64ARB(3, texMesh.first);
        for (auto mesh : texMesh.second) {
            glDrawElements(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT,
                    (void*)(mesh->indexOffset * sizeof(GLuint)));
        }
    }
}

