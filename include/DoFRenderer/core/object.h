#pragma once
#include "DoFRenderer/core/mesh.h"
#include "DoFRenderer/core/Texture.h"
#include "DoFRenderer/core/shader.h"
#include "DoFRenderer/core/light.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

namespace DoFRenderer {
	class Object {
	public:
		Object() { }
		Object(glm::vec3 translation, glm::vec3 rotationEuler, glm::vec3 scale);
		~Object();
		void loadModel(std::string modelPath);
		void prepareObject();
		void draw();
		void setShaderParams(const light* lightPtr, const camera* cameraPtr);
		void setShaderCameraParams(const camera* cameraPtr);
		void setShaderLightParams(const light* lightPtr);
		glm::mat4 getTranformation();
		glm::mat4 updateTranslation(glm::vec3 translation);
		glm::mat4 updateRotatation(glm::vec3 rotationEuler);
		glm::mat4 updateTransformation(glm::vec3 translation, glm::vec3 rotationEuler);
		void deleteBuffer();

		inline std::vector<Mesh *> getMeshes() const { return meshes; }
		inline shader* getShader() const { return shaderPtr; }

	private:
		std::string modelPath;
		glm::vec3 translation;
		glm::vec3 rotationEuler;
		glm::vec3 scale;
		glm::mat4 transformation;
		std::vector<Mesh *> meshes;
		shader* shaderPtr;

		int textureNumber = 0;
	};
}

