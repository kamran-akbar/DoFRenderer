#include "DoFRenderer/core/application.h"
#include "DoFRenderer/core/utils.h"
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
        windowPtr = std::make_unique<window>(window(RESOLUTION_X, 
            RESOLUTION_Y, "DoF Renderer"));
        windowPtr->createWindow();
        if (SCENE_NUM >= 1 && SCENE_NUM <= 3) {
            cameraPtr = std::make_unique<camera>(camera(
                50, windowPtr->getAspectRatio(1.0f), 0.1, 100.0f, 
                glm::vec3(0.0, 0.0f, -6.0f), glm::vec3(0.0f, 0.0f, 1.0f),
                glm::vec3(0.0f, 1.0f, 0.0f)
            ));
            cameraPtr->setLensVariable(cameraPtr->getNear(), 
                7.0f, 0.128f);
        }
        else if (SCENE_NUM == 4) {
            cameraPtr = std::make_unique<camera>(camera(
                50.0f, windowPtr->getAspectRatio(1.0f), 0.1, 1500.0f,
                glm::vec3(-43.0f, 40.0f, -230.0f), glm::vec3(0.55f, 0.0f, 1.0f),
                glm::vec3(0.0f, 1.0f, 0.0f)
            ));
            cameraPtr->setLensVariable(cameraPtr->getNear(),
                600.0f, 4.0f);
        }
        
        lightPtr = std::make_unique<light>(light(glm::vec3(0.0f, 75.0f, -100.0),
            glm::vec3(1.0f), glm::vec3(0.4f), glm::vec3(1.0f)));
        rendererPtr = std::make_unique<renderer>(renderer(windowPtr->getWidth(),
            windowPtr->getHeight(), layerCount));

        rendererPtr->generateFrameBuffers();
        if (ENABLE_DOF) {
            rendererPtr->prepareDepthDiscontinuity(cameraPtr.get());
            rendererPtr->prepareMerging(cameraPtr.get());
            rendererPtr->prepareSplatting(cameraPtr.get());
            rendererPtr->prepareSorting(cameraPtr.get());
            rendererPtr->prepareAccumulation();
        }
        rendererPtr->prepareRenderPassBuffers(cameraPtr.get(), lightPtr.get());
        rendererPtr->prepareScreenQuad();
    }
    
    void application::pipelineLoop() {
        rendererPtr->renderLoop(cameraPtr.get());
        rendererPtr->generateDepthDiscMap();
        if (ENABLE_DOF && frame > 2) {
            //isFrameChanged = true;
            rendererPtr->mergeFragments();
            rendererPtr->splatFragments();
            rendererPtr->sortFragments();
            rendererPtr->accumulateFragment();
        }
        rendererPtr->quadRenderLoop();
        /*if (frame == 4) {
            rendererPtr->storeFrame(true, "capture_AA_8.png");
        }*/
        /*this->sampleDenseParallelLightField(
            glm::vec3(5.0f, 0.0f, -7.5f),
            glm::vec3(-5.0f, 0.0f, -7.5f)
        );*/
        /*this->sampleDenseShearedLightField(
            glm::vec3(4.1f, 0.0f, -7.5f),
            glm::vec3(-4.1f, 0.0f, -7.5f),
            glm::vec3(-0.45f, 0.0f, 1.0f),
            glm::vec3(0.45f, 0.0f, 1.0f)
        );*/
        frame++;
    }

    void application::sampleDenseParallelLightField(
        glm::vec3 cameraStart, 
        glm::vec3 cameraEnd) {
        if (step < 1.0f) counter++;
        cameraPtr->interpPosition(cameraStart, cameraEnd, step);
        step += 0.0015f;
        step = glm::max(glm::min(step, 1.0f), 0.0f);
        storingFrame = step >= 1.0f ? false : true;
        counter = glm::min(counter, int(2.64f / 0.004f));
        std::stringstream ss;
        ss << std::setfill('0') << std::setw(4) << counter;
        std::cout << ss.str() << std::endl;
        rendererPtr->storeFrame(storingFrame, ss.str() + ".png");
        isFrameChanged = true;
    }

    void application::sampleDenseShearedLightField(
        glm::vec3 cameraStart,
        glm::vec3 cameraEnd,
        glm::vec3 forwardStart,
        glm::vec3 forwardEnd) {
        if (step < 1.0f) counter++;
        cameraPtr->interpPosition(
            cameraStart, cameraEnd, 
            glm::normalize(forwardStart),
            glm::normalize(forwardEnd), step
        );
        step += 0.0015f;
        step = glm::max(glm::min(step, 1.0f), 0.0f);
        storingFrame = step >= 1.0f ? false : true;
        counter = glm::min(counter, 660);
        std::stringstream ss;
        ss << std::setfill('0') << std::setw(4) << counter;
        std::cout << ss.str() << std::endl;
        rendererPtr->storeFrame(storingFrame, ss.str() + ".png");
        isFrameChanged = true;
    }

}