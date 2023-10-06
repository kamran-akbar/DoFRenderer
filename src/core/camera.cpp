#include "DoFRenderer/core/camera.h"
#include "DoFRenderer/core/utils.h"

#include <iostream>

namespace DoFRenderer {
	
	camera::camera() : fov(45.0f), aspectRatio(1.0f), near(0.1f), far(10.0f), position(glm::vec3(0.0f)),
		forward(glm::vec3(0.0f)), up(glm::vec3(0.0f)), viewMatrix(glm::mat4(1.0f)),
		projectionMatrix(glm::mat4(1.0f)) {}

	camera::camera(float aspectRatio, float near, float far, glm::vec3 position,
		 glm::vec3 forward, glm::vec3 up) : 
		aspectRatio(aspectRatio), near(near), far(far), position(position), 
		forward(glm::normalize(forward)), up(glm::normalize(up)) {
		viewMatrix = glm::lookAt(position, position + forward, up);
	}

	void camera::setLensVariable(float eyeLens, float focus, float aperture) {
		this->eyeLens = eyeLens;
		this->focusDist = focus;
		this->aperture = aperture * RESOLUTION_X / SENSOR_WIDTH;
		float focal = 1 / (1 / eyeLens + 1 / focus) * RESOLUTION_X / SENSOR_WIDTH;
		std::cout << 1 / (1 / eyeLens + 1 / focus) << std::endl;
		this->fov = glm::atan(RESOLUTION_X * 0.5f / focal) * 2;
		std::cout << this->fov << std::endl;
		this->projectionMatrix = glm::perspective(this->fov, this->aspectRatio,
			this->near, this->far);
	}

	void camera::setPosition(glm::vec3 position) {
		updateViewMatrix(position, this->forward, this->up);
	}

	void camera::setPositionForward(glm::vec3 position, glm::vec3 forward) {
		updateViewMatrix(position, forward, this->up);
	}

	void camera::interpPosition(glm::vec3 start, glm::vec3 end, float step) {
		step = glm::clamp(step, 0.0f, 1.0f);
		glm::vec3 newPosition = (1.0f - step) * start + step * end;
		setPosition(newPosition);
	}

	void camera::interpPosition(glm::vec3 start, glm::vec3 end, glm::vec3 startForward,
		glm::vec3 endForward, float step) {
		step = glm::clamp(step, 0.0f, 1.0f);
		glm::vec3 newPosition = (1.0f - step) * start + step * end;
		glm::vec3 newForward = (1.0f - step) * startForward + step * endForward;
		setPositionForward(newPosition, newForward);
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
		this->fov = glm::radians(fov);
		this->aspectRatio = aspectRatio;
		this->near = near;
		this->far = far;
		projectionMatrix = glm::perspective(fov, aspectRatio, near, far);
		return projectionMatrix;
	}
}