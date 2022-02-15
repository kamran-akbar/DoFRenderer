#pragma once

#include "glad/glad.h"
#include "glm/glm.hpp"

namespace DoFRenderer {
	class Quad {
	public:
		Quad();
		void bindVertexArray();
		void unbindVertexArray();
		void draw();
		void releaseMemory();
	private:
		unsigned int vertexArrayID;
		unsigned int vertexBufferID;
	};
}
