#include "DoFRenderer/core/application.h"

namespace DoFRenderer {

    application::~application() {
        windowPtr.reset();
        rendererPtr.reset();
        cameraPtr.reset();
        lightPtr.reset();
    }
	
	void application::run() {
        pipelineInitialization();
        while (!windowPtr->checkWindowClosed())
        {
            windowPtr->processInput();
            pipelineLoop();
            windowPtr->swapChainCall();
        }
        rendererPtr->releaseMemory();
        windowPtr->terminateWindow();
	}

    void application::pipelineInitialization() {
        windowPtr = std::make_unique<window>(window(1280, 800, "DoF Renderer"));
        windowPtr->createWindow();
        cameraPtr = std::make_unique<camera>(camera(45.0f, windowPtr->getAspectRatio(),
            1.0f, 100.0f, glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(0.0, 0.0f, 1.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)));
        //cameraPtr->setLensVariable(20, 50, 20);   
        cameraPtr->setLensVariable(20, 28.3519, 50);
        //cameraPtr->setLensVariable(20, 64.625, 50);
        //cameraPtr->setLensVariable(20, 70.6714, 50);
        lightPtr = std::make_unique<light>(light(glm::vec3(0.0f, 0.0f, 10.0),
            glm::vec3(1.0f), glm::vec3(0.4f), glm::vec3(1.0f)));
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
    }
    
    void application::pipelineLoop() {
        
        rendererPtr->renderLoop();
        rendererPtr->generateDepthDiscMap();
        rendererPtr->mergeFragments();
        rendererPtr->splatFragments();
        rendererPtr->sortFragments();
        rendererPtr->accumulateFragment();
        rendererPtr->quadRenderLoop();
    }
}