#pragma once
#include "DoFRenderer/core/window.h"
#include "DoFRenderer/core/renderer.h"
#include "DoFRenderer/core/shader.h"
#include "DoFRenderer/core/camera.h"
#include "DoFRenderer/core/light.h"
#include "glm/glm.hpp"


namespace DoFRenderer {
	class application {
	public:
		~application();
		void run();
		void pipelineInitialization();
		void pipelineLoop();
		void sampleDenseParallelLightField(
			glm::vec3 cameraStart, 
			glm::vec3 cameraEnd
		);
		void sampleDenseShearedLightField(
			glm::vec3 cameraStart, 
			glm::vec3 cameraEnd, 
			glm::vec3 forwardStart,
			glm::vec3 forwardEnd
		);

	private:
		static const unsigned int layerCount = 4;

		std::unique_ptr<window> windowPtr;
		std::unique_ptr<renderer> rendererPtr;
		std::unique_ptr<camera> cameraPtr;
		std::unique_ptr<light> lightPtr;
		float step = 0.0f;
		bool storingFrame = false;
		int counter = -1;
	};

}
