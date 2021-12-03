#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace DoFRenderer {
	class camera {
	public:
		camera();
		camera(float fov, float aspectRatio, float near, float far, glm::vec3 position, glm::vec3 forward, glm::vec3 up);
		
		~camera() {}
		
		glm::vec3 getCameraPosition() const;
		glm::mat4 getViewMatrix() const;
		glm::mat4 getProjectionMatrix() const;
		glm::mat4 updateViewMatrix(glm::vec3 position, glm::vec3 forward, glm::vec3 up);
		glm::mat4 updateProjectionMatrix(float fov, float aspectRatio, float near, float far);
		
	private:
		float fov;
		float aspectRatio;
		float near, far;
		glm::vec3 position;
		glm::vec3 forward, up;
		glm::mat4 viewMatrix;
		glm::mat4 projectionMatrix;
	};
}