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
        
        cameraPtr = std::make_unique<camera>(camera(45.0f, windowPtr->getAspectRatio(), 
            1.0f, 100.0f, glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(0.0, 0.0f, 1.0f), 
            glm::vec3(0.0f, 1.0f, 0.0f)));
        cameraPtr->setLensVariable(4.0, 10.0, 30);
        
        lightPtr = std::make_unique<light>(light(glm::vec3(0.0f, 5.0f, -2.0), glm::vec3(1.0f), glm::vec3(0.4f),
            glm::vec3(1.0f)));
        
        rendererPtr = std::make_unique<renderer>(renderer(windowPtr->getWidth(), 
            windowPtr->getHeight(), layerCount));
        
        rendererPtr->generateFrameBuffers();
        rendererPtr->prepareDepthDiscontinuity(cameraPtr.get());
        rendererPtr->prepareMerging(cameraPtr.get());
        rendererPtr->prepareSplatting(cameraPtr.get());
        rendererPtr->prepareSorting(cameraPtr.get());
        rendererPtr->prepareAccumulation();
        rendererPtr->prepareRenderPassBuffers(cameraPtr.get(), lightPtr.get());
        rendererPtr->prepareScreenQuad();

        while (!windowPtr->checkWindowClosed())
        {
            windowPtr->processInput();
            rendererPtr->renderLoop();
            rendererPtr->generateDepthDiscMap();
            rendererPtr->mergeFragments();
            rendererPtr->splatFragments();
            rendererPtr->sortFragments(); 
            rendererPtr->accumulateFragment();
            rendererPtr->quadRenderLoop();
            windowPtr->swapChainCall();
        }
        rendererPtr->deleteBuffers();
        windowPtr->terminateWindow();
	}
}