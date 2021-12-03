#include "DoFRenderer/core/light.h"

namespace DoFRenderer {
	light::light() : position(glm::vec3(0.0)), diffuse(glm::vec3(0.0)), ambient(glm::vec3(0.0)),
		specular(glm::vec3(0.0)) { }

	light::light(glm::vec3 position, glm::vec3 diffuse, glm::vec3 ambient, glm::vec3 specular) :
		position(position), diffuse(diffuse), ambient(ambient), specular(specular) { }

	glm::vec3 light::getPosition() const{
		return position;
	}

	glm::vec3 light::getAmbient() const{
		return ambient;
	}

	glm::vec3 light::getDiffuse() const{
		return diffuse;
	}

	glm::vec3 light::getSpecular() const{
		return specular;
	}

}