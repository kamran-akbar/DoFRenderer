#include "DoFRenderer/core/application.h"
#include <string>
#include <sstream>
#include <iomanip>

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
        windowPtr = std::make_unique<window>(window(1280, 720, "DoF Renderer"));
        windowPtr->createWindow();
        cameraPtr = std::make_unique<camera>(camera(45.0f, windowPtr->getAspectRatio(),
            1.0f, 100.0f, glm::vec3(0.0, 0.0f, -3.0f), glm::vec3(0.0, 0.0f, 1.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)));
        //cameraPtr->setLensVariable(20, 10, 50);   
        //cameraPtr->setLensVariable(20, 28.3519, 50);
        cameraPtr->setLensVariable(20, 64.625, 50);
        //cameraPtr->setLensVariable(20, 84.6714, 50);
        lightPtr = std::make_unique<light>(light(glm::vec3(0.0f, 5.0f, -10.0),
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
        rendererPtr->renderLoop(cameraPtr.get());
        rendererPtr->generateDepthDiscMap();
        rendererPtr->mergeFragments();
        rendererPtr->splatFragments();
        rendererPtr->sortFragments();
        rendererPtr->accumulateFragment();
        rendererPtr->quadRenderLoop();
        //this->sampleDenseLightField(glm::vec3(1.32f, 0.0f, -3.0f),
        //    glm::vec3(-1.32f, 0.0f, -3.0f));
    }

    void application::sampleDenseLightField(glm::vec3 cameraStart, glm::vec3 cameraEnd) {
        if (step < 1.0f) counter++;
        cameraPtr->interpPosition(glm::vec3(-1.32f, 0.0f, -3.0f),
            glm::vec3(1.32f, 0.0f, -3.0f), step);
        step += 0.0015f;
        step = glm::max(glm::min(step, 1.0f), 0.0f);
        storingFrame = step >= 1.0f ? false : true;
        counter = glm::min(counter, int(2.64f / 0.004f));
        std::stringstream ss;
        ss << std::setfill('0') << std::setw(4) << counter;
        std::cout << ss.str() << std::endl;
        rendererPtr->storeFrame(storingFrame, ss.str() + ".png");
    }
}