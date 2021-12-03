#pragma once
#include "glm/glm.hpp"

namespace DoFRenderer {
	class light {
	public:
		light();
		light(glm::vec3 position, glm::vec3 diffuse, glm::vec3 ambient, glm::vec3 specular);
		glm::vec3 getPosition() const;
		glm::vec3 getDiffuse() const;
		glm::vec3 getAmbient() const;
		glm::vec3 getSpecular() const;

	private:
		glm::vec3 position;
		glm::vec3 diffuse;
		glm::vec3 ambient;
		glm::vec3 specular;
	};
}