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
		void sampleHorizontalParallaxLightField(
			glm::vec3 cameraStart, 
			glm::vec3 cameraEnd
		);
		void sampleFullParallaxLightfield(
			glm::vec3 cameraStart,
			glm::vec3 cameraEnd
		);

	private:
		static const unsigned int layerCount = 4;

		std::unique_ptr<window> windowPtr;
		std::unique_ptr<renderer> rendererPtr;
		std::unique_ptr<camera> cameraPtr;
		std::unique_ptr<light> lightPtr;
		glm::vec3 step = glm::vec3(0.004f);
		bool storingFrame = false;
		bool isFrameChanged = false;
		float accTime = 0.0f;
		float timeAverage = 0.0f;
		unsigned int frame = 0;
		glm::ivec2 counter = glm::ivec2(0);

		std::vector<glm::vec3> cameraPositions{
			glm::vec3(0.1f, 0.0f, -6.0f),
			glm::vec3(-0.1f, 0.0f, -6.0f),
			glm::vec3(0.0f, 0.1f, -6.0f),
			glm::vec3(0.0f, -0.1f, -6.0f)
		};
		float camNum = 0;
	};

}
