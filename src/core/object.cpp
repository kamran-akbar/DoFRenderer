#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader/tiny_obj_loader.h>
#include "DoFRenderer/core/object.h"
#include <iostream>
#include <unordered_map>
#include <algorithm>

namespace DoFRenderer {
    
    Object::Object(glm::vec3 translation, glm::vec3 rotationEuler, glm::vec3 scale) {
        shaderPtr = new shader("../src/shaders/vertexShader.vert",
            "../src/shaders/geometryShader.geom", "../src/shaders/fragmentShader.frag");
        
        this->translation = translation;
        this->rotationEuler = rotationEuler;
        transformation = glm::mat4(1.0f);
        transformation = glm::scale(transformation, scale);
        transformation = glm::translate(transformation, translation);
        rotationEuler = glm::radians(rotationEuler);
        transformation = glm::rotate(transformation, rotationEuler.x, glm::vec3(1.0f, 0.0f, 0.0f));
        transformation = glm::rotate(transformation, rotationEuler.y, glm::vec3(0.0f, 1.0f, 0.0f));
        transformation = glm::rotate(transformation, rotationEuler.z, glm::vec3(0.0f, 0.0f, 1.0f));
    }

    Object::~Object() {
        delete shaderPtr;
        for (Mesh* mesh : meshes) {
            delete mesh;
        }
    }

    void Object::loadModel(std::string modelPath) {
        tinyobj::ObjReaderConfig reader_config;
        reader_config.mtl_search_path = "../models/";
        tinyobj::ObjReader reader;
        
        if (!reader.ParseFromFile(modelPath, reader_config)) {
            if (!reader.Error().empty()) {
                std::cerr << "TinyObjReader: " << reader.Error();
            }
            exit(1);
        }

        if (!reader.Warning().empty()) {
            std::cout << "TinyObjReader: " << reader.Warning();
        }

        auto& attrib = reader.GetAttrib();
        auto& shapes = reader.GetShapes();
        auto& materials = reader.GetMaterials();

        std::unordered_map<Vertex, unsigned int> uniqueVertices;
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::unordered_map<int, Material*> allMaterials;
        std::unordered_map<int, Texture*> allTextures;
        int material_id;

        // Loop over shapes
        for (size_t s = 0; s < shapes.size(); s++) {
            size_t index_offset = 0;
            
            vertices.clear();
            uniqueVertices.clear();
            indices.clear();

            material_id = -1;

            // Loop over faces(polygon)
            for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
                size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

                // Loop over vertices in the face.
                for (size_t v = 0; v < fv; v++) {
                    Vertex vertex;
                    // access to vertex
                    tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                    vertex.position = {
                        attrib.vertices[3 * size_t(idx.vertex_index) + 0],
                        attrib.vertices[3 * size_t(idx.vertex_index) + 1],
                        attrib.vertices[3 * size_t(idx.vertex_index) + 2]
                    };

                    // Check if `normal_index` is zero or positive. negative = no normal data
                    if (idx.normal_index >= 0) {
                        vertex.normal = {
                            attrib.normals[3 * size_t(idx.normal_index) + 0],
                            attrib.normals[3 * size_t(idx.normal_index) + 1],
                            attrib.normals[3 * size_t(idx.normal_index) + 2]
                        };
                    }

                    // Check if texcoord_index is zero or positive. negative = no texcoord data
                    if (idx.texcoord_index >= 0) {
                        vertex.uv = {
                            attrib.texcoords[2 * size_t(idx.texcoord_index) + 0],
                            attrib.texcoords[2 * size_t(idx.texcoord_index) + 1]
                        };
                    }
                    // vertex colors
                    vertex.color = {
                        attrib.colors[3 * size_t(idx.vertex_index) + 0],
                        attrib.colors[3 * size_t(idx.vertex_index) + 1],
                        attrib.colors[3 * size_t(idx.vertex_index) + 2]
                    };
                    if (uniqueVertices.count(vertex) == 0) {
                        uniqueVertices[vertex] = vertices.size();
                        vertices.push_back(vertex);
                    }
                  
                    indices.push_back(uniqueVertices[vertex]);
                }
                index_offset += fv;

                // per-face material
                material_id = shapes[s].mesh.material_ids[f];
            }
            meshes.push_back(new Mesh(vertices, indices));
            glm::vec3 ambient = {
                materials[material_id].ambient[0],
                materials[material_id].ambient[1],
                materials[material_id].ambient[2]
            };
            glm::vec3 diffuse = {
                materials[material_id].diffuse[0],
                materials[material_id].diffuse[1],
                materials[material_id].diffuse[2]
            };
            glm::vec3 specular = {
                materials[material_id].specular[0],
                materials[material_id].specular[1],
                materials[material_id].specular[2]
            };
            float shininess = materials[material_id].shininess;
            if (allMaterials.find(material_id) == allMaterials.end()) {
                allMaterials[material_id] = new Material(ambient, diffuse,
                    specular, shininess);
            }
            if (materials[material_id].diffuse_texname != "") {
                if (allTextures.find(material_id) == allTextures.end()) {
                    allTextures[material_id] = new Texture(
                        "diffuseTexture",
                        materials[material_id].diffuse_texname,
                        textureNumber,
                        GL_REPEAT,
                        GL_LINEAR,
                        GL_RGBA,
                        GL_RGBA,
                        GL_UNSIGNED_BYTE
                    );
                    textureNumber++;
                }
                meshes[meshes.size() - 1]->setTexture(allTextures[material_id]);
            }
            meshes[meshes.size() - 1]->setMaterial(allMaterials[material_id]);
            
        }

    }
    
    void Object::prepareObject() {
        for (int i = 0; i < meshes.size(); i++) {
            meshes[i]->prepareObject(shaderPtr);
        }
    }

    void Object::draw() {
        for (int i = 0; i < meshes.size(); i++) {
            shaderPtr->setMat4("model", getTranformation());
            meshes[i]->draw(shaderPtr);
        }
    }

    void Object::setShaderParams(const light* lightPtr, const camera* cameraPtr) {
        for (int i = 0; i < meshes.size(); i++) {
            shaderPtr->use();
            meshes[i]->setShaderParams(lightPtr, cameraPtr, shaderPtr);
        }
    }

    void Object::setShaderCameraParams(const camera* cameraPtr) {
        for (int i = 0; i < meshes.size(); i++) {
            shaderPtr->use();
            meshes[i]->setShaderParams(nullptr, cameraPtr, shaderPtr);
        }
    }

    void Object::setShaderLightParams(const light* lightPtr) {
        for (int i = 0; i < meshes.size(); i++) {
            shaderPtr->use();
            meshes[i]->setShaderParams(lightPtr, nullptr, shaderPtr);
        }
    }

    glm::mat4 Object::getTranformation() {
        return transformation;
    }

    glm::mat4 Object::updateTranslation(glm::vec3 translation) {
        transformation = glm::translate(transformation, translation);
        return transformation;
    }

    glm::mat4 Object::updateRotatation(glm::vec3 rotationEuler) {
        rotationEuler = glm::radians(rotationEuler);
        transformation = glm::rotate(transformation, rotationEuler.x, glm::vec3(1.0f, 0.0f, 0.0f));
        transformation = glm::rotate(transformation, rotationEuler.y, glm::vec3(0.0f, 1.0f, 0.0f));
        transformation = glm::rotate(transformation, rotationEuler.z, glm::vec3(0.0f, 0.0f, 1.0f));
        return transformation;
    }

    glm::mat4 Object::updateTransformation(glm::vec3 translation, glm::vec3 rotationEuler) {
        transformation = glm::translate(transformation, translation);
        rotationEuler = glm::radians(rotationEuler);
        transformation = glm::rotate(transformation, rotationEuler.x, glm::vec3(1.0f, 0.0f, 0.0f));
        transformation = glm::rotate(transformation, rotationEuler.y, glm::vec3(0.0f, 1.0f, 0.0f));
        transformation = glm::rotate(transformation, rotationEuler.z, glm::vec3(0.0f, 0.0f, 1.0f));
        return transformation;
    }

    void Object::deleteBuffer() {
        for (int i = 0; i < meshes.size(); i++) {
            meshes[i]->deleteBuffers();
        }
    }

}