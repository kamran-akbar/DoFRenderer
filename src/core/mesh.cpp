#include "DoFRenderer/core/mesh.h"

namespace DoFRenderer {
	Mesh::Mesh() { }
	
	Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
		std::vector<std::string> texturePaths) { 
		this->vertices.assign(vertices.begin(), vertices.end());
		this->indices.assign(indices.begin(), indices.end());
		this->texturePaths.assign(texturePaths.begin(), texturePaths.end());

		shaderPtr = new shader("../src/shaders/vertexShader.vert",
					"../src/shaders/fragmentShader.frag");
	}
	
	Mesh::~Mesh() {
		delete material;
		delete shaderPtr;
	}
	
	void Mesh::prepareObject() { 
		glGenVertexArrays(1, &vertexArray);
		glGenBuffers(1, &vertexBuffer);
		glGenBuffers(1, &elementBuffer);

		glBindVertexArray(vertexArray);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);

		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, color)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, uv)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, normal)));
		glEnableVertexAttribArray(3);
		for (int i = 0; i < texturePaths.size(); i++) {
			textures.push_back(Texture(texturePaths[i], GL_REPEAT, GL_LINEAR, GL_RGB, GL_RGB,
				GL_UNSIGNED_BYTE));
		}
	}
	
	void Mesh::draw() {
		setShaderMaterialParams();

		for (int i = 0; i < textures.size(); i++) {
			textures[i].bind(i);
		}
		glBindVertexArray(vertexArray);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);	
	}
	
	void Mesh::setMaterial(const Material* material) { 
		this->material = new Material();
		this->material->ambient = material->ambient;
		this->material->diffuse = material->diffuse;
		this->material->specular = material->specular;
		this->material->shininess = material->shininess;
	}
	
	void Mesh::setShaderParams(const light* lightPtr, const camera* cameraPtr) {
		shaderPtr->use();

		shaderPtr->setMat4("view", cameraPtr->getViewMatrix());
		shaderPtr->setMat4("projection", cameraPtr->getProjectionMatrix());
		shaderPtr->setVec3("cameraPos", cameraPtr->getCameraPosition());

		glm::vec3 position = lightPtr->getPosition();
		glm::vec3 ambient = lightPtr->getAmbient();
		glm::vec3 diffuse = lightPtr->getDiffuse();
		glm::vec3 specular = lightPtr->getSpecular();
		shaderPtr->setVec3("light.position", position.x, position.y, position.z);
		shaderPtr->setVec3("light.ambient", ambient.x, ambient.y, ambient.z);
		shaderPtr->setVec3("light.diffuse", diffuse.x, diffuse.y, diffuse.z);
		shaderPtr->setVec3("light.specular", specular.x, specular.y, specular.z);
	}

	void Mesh::setShaderMaterialParams() {
		shaderPtr->setVec3("material.ambient", material->ambient.x, material->ambient.y, material->ambient.z);
		shaderPtr->setVec3("material.diffuse", material->diffuse.x, material->diffuse.y, material->diffuse.z);
		shaderPtr->setVec3("material.specular", material->specular.x, material->specular.y, material->specular.z);
		shaderPtr->setFloat("material.shininess", material->shininess);
	}
	
	void Mesh::deleteBuffers() { 
		glDeleteVertexArrays(1, &vertexArray);
		glDeleteBuffers(1, &vertexBuffer);
		glDeleteBuffers(1, &elementBuffer);
	}
}