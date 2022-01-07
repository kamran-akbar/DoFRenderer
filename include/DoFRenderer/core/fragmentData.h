#pragma once
#include "glm/glm.hpp"

struct fragmentColorDepth {
	glm::vec3 color;
	float depth;
};

struct fragmentMergingData {
	unsigned int mergeID;
	glm::uvec2 screenCoords;
};