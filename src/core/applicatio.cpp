#include "DoFRenderer/core/application.h"

namespace DoFRenderer {

    application::~application() {
        windowPtr.reset();
        rendererPtr.reset();
        cameraPtr.reset();
        lightPtr.reset();
    }
	
	void application::run() {
        
        windowPtr = std::make_unique<window>(window(1280, 800, "DoF Renderer"));
        windowPtr->createWindow();
        
        cameraPtr = std::make_unique<camera>(camera(45.0f, windowPtr->getAspectRatio(), 0.01f, 10.0f,
            glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(0.0, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
        
        lightPtr = std::make_unique<light>(light(glm::vec3(0.0f, 0.5f, -0.5f), glm::vec3(1.0f), glm::vec3(0.2f),
            glm::vec3(1.0f)));
        
        rendererPtr = std::make_unique<renderer>(renderer());
        
        rendererPtr->prepareRenderPassBuffers(cameraPtr.get(), lightPtr.get());

        while (!windowPtr->checkWindowClosed())
        {
            windowPtr->processInput();
            rendererPtr->renderLoop();
            windowPtr->swapChainCall();
        }
        rendererPtr->deleteBuffers();
        windowPtr->terminateWindow();
	}
}