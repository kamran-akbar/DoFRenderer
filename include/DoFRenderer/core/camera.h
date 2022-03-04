#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace DoFRenderer {
	class camera {
	public:
		camera();
		camera(float fov, float aspectRatio, float near, float far, glm::vec3 position, 
			glm::vec3 forward, glm::vec3 up);
		camera(float fov, float aspectRatio, float near, float far, glm::vec3 position,
			float focal, float focus, int aperture, glm::vec3 forward, glm::vec3 up);
		~camera() {}

		void setLensVariable(float focal, float focus, int aperture);
		void setPosition(glm::vec3 position);
		void setPositionForward(glm::vec3 position, glm::vec3 forward);
		void interpPosition(glm::vec3 start, glm::vec3 end, float step);
		void interpPosition(glm::vec3 start, glm::vec3 end, glm::vec3 startForward,
			glm::vec3 endForward, float step);

		glm::vec3 getCameraPosition() const;
		glm::mat4 getViewMatrix() const;
		glm::mat4 getProjectionMatrix() const;
		glm::mat4 updateViewMatrix(glm::vec3 position, glm::vec3 forward, glm::vec3 up);
		glm::mat4 updateProjectionMatrix(float fov, float aspectRatio, float near, float far);

		inline float getFoV() const { return fov; }
		inline float getNear() const { return near; }
		inline float getFar() const { return far; }
		inline float getEyeLens() const { return eyeLens; }
		inline float getFocusDist() const { return focusDist; }
		inline int getAperture() const { return aperture; }
		
	private:
		float fov;
		float aspectRatio;
		float near, far;
		float eyeLens, focusDist;
		int aperture;
		glm::vec3 position;
		glm::vec3 forward, up;
		glm::mat4 viewMatrix;
		glm::mat4 projectionMatrix;
	};
}