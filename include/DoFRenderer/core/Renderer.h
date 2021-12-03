#pragma once
#include "DoFRenderer/core/object.h"
#include "DoFRenderer/core/camera.h"
#include "DoFRenderer/core/shader.h"
#include "DoFRenderer/core/light.h"
#include "DoFRenderer/core/time.h"
#include <iostream>

namespace DoFRenderer {
	

	class renderer {
	
	public:
		renderer();
		~renderer();
		void prepareRenderPassBuffers(const camera* cameraPtr, const light* lightPtr);
		void renderLoop();
		void deleteBuffers();
	private:
		std::vector<Object*> objects;
		Timer* timer;
	};
}