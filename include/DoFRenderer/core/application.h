#pragma once
#include "DoFRenderer/core/window.h"
#include "DoFRenderer/core/renderer.h"
#include "DoFRenderer/core/shader.h"
#include "DoFRenderer/core/camera.h"
#include "DoFRenderer/core/light.h"


namespace DoFRenderer {
	class application {
	public:
		~application();
		void run();
		void pipelineInitialization();
		void pipelineLoop();

	private:
		static const unsigned int layerCount = 4;

		std::unique_ptr<window> windowPtr;
		std::unique_ptr<renderer> rendererPtr;
		std::unique_ptr<camera> cameraPtr;
		std::unique_ptr<light> lightPtr;

		int once = 0;
	};

}
