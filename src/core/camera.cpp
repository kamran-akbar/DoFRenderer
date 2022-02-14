#include "DoFRenderer/core/camera.h"

namespace DoFRenderer {
	
	camera::camera() : fov(45.0f), aspectRatio(1.0f), near(0.1f), far(10.0f), position(glm::vec3(0.0f)),
		forward(glm::vec3(0.0f)), up(glm::vec3(0.0f)), viewMatrix(glm::mat4(1.0f)),
		projectionMatrix(glm::mat4(1.0f)) {}

	camera::camera(float fov, float aspectRatio, float near, float far, glm::vec3 position, glm::vec3 forward,
		glm::vec3 up) : fov(fov), aspectRatio(aspectRatio), near(near), far(far), position(position),
		forward(glm::normalize(forward)), up(glm::normalize(up)) {
		viewMatrix = glm::lookAt(position, position + forward, up);
		projectionMatrix = glm::perspective(fov, aspectRatio, near, far);
	}

	camera::camera(float fov, float aspectRatio, float near, float far, glm::vec3 position,
		float eyeLens, float focus, int aperture, glm::vec3 forward, glm::vec3 up) : 
		fov(fov), aspectRatio(aspectRatio), near(near), far(far), position(position),
		eyeLens(eyeLens), focusDist(focus), aperture(aperture), forward(glm::normalize(forward)), 
		up(glm::normalize(up)) {
		viewMatrix = glm::lookAt(position, position + forward, up);
		projectionMatrix = glm::perspective(fov, aspectRatio, near, far);
	}

	void camera::setLensVariable(float eyeLens, float focus, int aperture) {
		this->eyeLens = eyeLens;
		this->focusDist = focus;
		this->aperture = aperture;
	}

	glm::vec3 camera::getCameraPosition() const{
		return position;
	}

	glm::mat4 camera::getViewMatrix() const{
		return viewMatrix;
	}

	glm::mat4 camera::getProjectionMatrix() const{
		return projectionMatrix;
	}

	glm::mat4 camera::updateViewMatrix(glm::vec3 position, glm::vec3 forward, glm::vec3 up) {
		this->position = position;
		this->forward = forward;
		this->up = up;
		viewMatrix = glm::lookAt(position, position + forward, up);
		return viewMatrix;
	}

	glm::mat4 camera::updateProjectionMatrix(float fov, float aspectRatio, float near, float far) {
		this->fov = fov;
		this->aspectRatio = aspectRatio;
		this->near = near;
		this->far = far;
		projectionMatrix = glm::perspective(fov, aspectRatio, near, far);
		return projectionMatrix;
	}
}