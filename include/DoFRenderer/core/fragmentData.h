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

struct fragmentData {
	unsigned int red_green;
	unsigned int blue_mergeID;
	float depth;
};