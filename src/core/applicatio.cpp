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
        glm::vec3 a, b;
        if (SCENE_NUM == 1 && SCENE_NUM == 2) {
            cameraPtr = std::make_unique<camera>(camera(
                windowPtr->getAspectRatio(1.0f), 0.125, 100.0f, 
                CAMERA_START, glm::vec3(0.0f, 0.0f, 1.0f),
                glm::vec3(0.0f, 1.0f, 0.0f)
            ));
            cameraPtr->setLensVariable(cameraPtr->getNear(), 6.0f, 0.112f);
        }
        else if (SCENE_NUM == 3) {
            cameraPtr = std::make_unique<camera>(camera(
                windowPtr->getAspectRatio(1.0f), 0.125, 100.0f,
                CAMERA_POS, glm::vec3(0.0f, 0.0f, 1.0f),
                glm::vec3(0.0f, 1.0f, 0.0f)
            ));
            cameraPtr->setLensVariable(cameraPtr->getNear(), 7.0f, BASELINE * 2);
        }
        else if (SCENE_NUM == 4) {
            cameraPtr = std::make_unique<camera>(camera(
                windowPtr->getAspectRatio(1.0f), 0.1, 1500.0f,
                glm::vec3(-43.0f, 40.0f, -230.0f), glm::vec3(0.55f, 0.0f, 1.0f),
                glm::vec3(0.0f, 1.0f, 0.0f)
            ));
            cameraPtr->setLensVariable(cameraPtr->getNear(), 300.0f, 4.0f);
        }
        else if (SCENE_NUM == 5) {
            cameraPtr = std::make_unique<camera>(camera(
                windowPtr->getAspectRatio(1.0f), 0.125, 100.0f,
                CAMERA_POS, glm::vec3(0.0f, 0.0f, 1.0f),
                glm::vec3(0.0f, 1.0f, 0.0f)
            ));
            cameraPtr->setLensVariable(cameraPtr->getNear(), 6.0f, BASELINE * 2);
        }
        else if (SCENE_NUM == 6) {
            cameraPtr = std::make_unique<camera>(camera(
                windowPtr->getAspectRatio(1.0f), 0.125, 100.0f,
                CAMERA_POS, glm::vec3(0.0f, 0.0f, 1.0f),
                glm::vec3(0.0f, 1.0f, 0.0f)
            ));
            cameraPtr->setLensVariable(cameraPtr->getNear(), 6.0f, BASELINE * 2);
        }
        
        lightPtr = std::make_unique<light>(light(glm::vec3(0.0f, 20.0f, -100.0),
            glm::vec3(1.0f), glm::vec3(0.2f), glm::vec3(0.0f)));
        rendererPtr = std::make_unique<renderer>(renderer(windowPtr->getWidth(),
            windowPtr->getHeight(), layerCount));

        rendererPtr->generateFrameBuffers();
        rendererPtr->prepareDepthDiscontinuity(cameraPtr.get());
        if (ENABLE_DOF) {
            rendererPtr->prepareMerging(cameraPtr.get());
            rendererPtr->prepareSplatting(cameraPtr.get());
            rendererPtr->prepareSorting(cameraPtr.get());
            rendererPtr->prepareAccumulation(cameraPtr.get());
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
        float time = rendererPtr->quadRenderLoop();

        if (STORE_FRAME && frame == 4) {
            rendererPtr->storeFrame(STORE_FRAME, "DoF_mid_big_aperture.png");
        }
        if (STORE_FRAMES && frame == 4) {
            rendererPtr->storeFrames(STORE_FRAMES, "view", ".png");
        }
        if (SAMPLE_HORIZONTAL_ONLY_PARALLAX_LIGHT_FIELD) {
            this->sampleHorizontalParallaxLightField(
                glm::vec3(-1.32f, 0.0f, -6.0f),
                glm::vec3(1.32f, 0.0f, -6.0f)
            );
        }
        if (SAMPLE_FULL_PARALLAX_LIGHT_FIELD) {
            this->sampleFullParallaxLightfield(
                CAMERA_START, CAMERA_END
            );
        }
        frame++;
        if (TIME_AVERAGE) {
            if (frame > 5 && frame <= 10 && camNum <= cameraPositions.size()) {
                accTime += time;
            }
            else if (frame > 10 && camNum <= cameraPositions.size()) {
                std::cout << "camera number : " << camNum - 1 << std::endl;
                std::cout << "accumulated time : " << accTime / (frame - 5) << " Last time stance: " << time << std::endl;
                timeAverage += accTime / (frame - 5);
                frame = 0;
                accTime = 0.0f;
                if (camNum < cameraPositions.size()) {
                    cameraPtr->setPosition(cameraPositions[camNum]);
                }
                camNum++;
            }
            if (camNum > cameraPositions.size()) {
                std::cout << "Final time average is: " << timeAverage / (cameraPositions.size() + 1) << std::endl;
            }
        }
    }

    void application::sampleHorizontalParallaxLightField(
        glm::vec3 cameraStart, glm::vec3 cameraEnd) {
        if (counter.x >= int(abs(cameraStart.x - cameraEnd.x) / step.x) + 1) {
            std::cout << "camera exceed the baseline!" << std::endl;
            return;
        }
        else {
            storingFrame = true;
            cameraPtr->setPosition(cameraStart + glm::vec3(counter.x, counter.y, 0.0f) * step);
            std::stringstream ss;
            ss << std::setfill('0') << std::setw(4) << counter.x;
            std::cout << ss.str() << std::endl;
            rendererPtr->storeFrame(storingFrame, ss.str() + ".png");
            isFrameChanged = true;
            counter.x++;
        }
    }


    void application::sampleFullParallaxLightfield(
            glm::vec3 cameraStart, glm::vec3 cameraEnd) {
        float depth = cameraStart.z;
        glm::vec3 bottomLeft = glm::vec3(cameraStart.x, cameraStart.y, depth);
        glm::vec3 bottomRight = glm::vec3(cameraEnd.x, cameraStart.y, depth);
        glm::vec3 topLeft = glm::vec3(cameraStart.x, cameraEnd.y, depth);
        glm::vec3 topRight = glm::vec3(cameraEnd.x, cameraEnd.y, depth);

        int cameraNumX = int(abs(cameraStart.x - cameraEnd.x) / step.x) + 1;
        int cameraNumY = int(abs(cameraStart.y - cameraEnd.y) / step.y);

        glm::vec3 currentPosition = bottomLeft;
        if (counter.y > cameraNumY) {
            std::cout << "camera exceed the vertical baseline!" << std::endl;
            return;
        }
        else if (counter.x > cameraNumX) {
            counter = glm::ivec2(0, counter.y + 1);
            currentPosition = bottomLeft;
            cameraPtr->setPosition(currentPosition + glm::vec3(counter.x, counter.y, 0.0f) * step);
        }
        else if(counter.y > -1){
            storingFrame = true;
            cameraPtr->setPosition(currentPosition + glm::vec3(counter.x, counter.y, 0.0f) * step);
            std::stringstream ss;
            ss << "V_" << std::setfill('0') << std::setw(4) << counter.y << "_H_" << std::setfill('0') << std::setw(4) << counter.x;
            std::cout << ss.str() << std::endl;
            rendererPtr->storeFrame(storingFrame, ss.str() + ".png");
            isFrameChanged = true;
            counter.x++;
        }
        else {
            counter.x++;
            cameraPtr->setPosition(currentPosition + glm::vec3(counter.x, counter.y, 0.0f) * step);
        }
    }
}