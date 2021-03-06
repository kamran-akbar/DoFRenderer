#pragma once
#include "DoFRenderer/core/Texture.h"
#include "DoFRenderer/core/shader.h"
#include "DoFRenderer/core/light.h"
#include "DoFRenderer/core/camera.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>
#include <vector>
#include <unordered_map>

namespace DoFRenderer {
	class Vertex {
	public:
		Vertex() : position(glm::vec3(0.0f)), color(glm::vec3(0.0f)), uv(glm::vec2(0.0f)) {}
		glm::vec3 position;
		glm::vec3 color;
		glm::vec2 uv;
		glm::vec3 normal;

		bool operator==(const Vertex& other) const {
			return position == other.position && color == other.color && uv == other.uv;
		}
	};

	class Material {
	public:
		Material() : ambient(glm::vec3(0.0f)), diffuse(glm::vec3(0.0f)), specular(glm::vec3(0.0f)),
			shininess(0.0f) { }
		Material(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess) :
			ambient(ambient), diffuse(diffuse), specular(specular), shininess(shininess) { }
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		float shininess;
	};

	class Mesh {
	public:
		Mesh();
		Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
		~Mesh();
		void prepareObject(const shader* shaderPtr);
		void draw(const shader* shaderPtr);
		void setMaterial(const Material* material);
		void setTexture(const Texture* texture);
		void setShaderParams(const light* lightPtr, const camera* cameraPtr, const shader* shaderPtr);
		void deleteBuffers();
		inline Material* getMaterial() const { return material; }
		inline int getTextureNumber() { return textures.size(); }
	
	private:
		void setShaderMaterialParams(const shader* shaderPtr);

		unsigned int vertexBuffer;
		unsigned int vertexArray;
		unsigned int elementBuffer;
		std::vector<Vertex> vertices;
		std::vector<Texture *> textures;
		std::vector<unsigned int> indices;
		Material* material;
	};

}

namespace std {
	template<> struct hash<DoFRenderer::Vertex> {
		size_t operator()(DoFRenderer::Vertex const& vertex) const {
			return ((hash<glm::vec3>()(vertex.position) ^
				(hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
				(hash<glm::vec2>()(vertex.uv) << 1);
		}
	};
}