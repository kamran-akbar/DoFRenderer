#include "DoFRenderer/core/Renderer.h"
#include "DoFRenderer/core/mesh.h"
#include "DoFRenderer/core/utils.h"

namespace DoFRenderer {

    renderer::renderer() : windowWidth(0), windowHeight(0), layerCount(0) {
        timer = new Timer();
    }

    renderer::renderer(unsigned int width, unsigned int height, unsigned int layerCount) :
        windowWidth(width), windowHeight(height), layerCount(layerCount) {
        timer = new Timer();
    }
    
    renderer::~renderer() {
        for (auto attachment : attachments) {
            delete attachment.second;
        }
        for (auto texture : textures) {
            delete texture.second;
        }
        for (auto shader : shaders) {
            delete shader.second;
        }
        for (auto buffer : buffers) {
            delete buffer.second;
        }
        for (Object* obj : objects) {
            delete obj;
        }
        for (auto frameBuffer : frameBuffers) {
            delete frameBuffer.second;
        }
        for (auto quad : quads) {
            delete quad.second;
        }
    }

    void renderer::generateFrameBuffers() {
        attachments[COLOR_ATTACHMENT] = new Texture2DArray(
            GL_RGBA8, 1, windowWidth, windowHeight, layerCount, GL_REPEAT, GL_LINEAR
        );
        
        attachments[DEPTH_ATTACHMENT] = new Texture2DArray(
            GL_DEPTH_COMPONENT24, 1, windowWidth, windowHeight, layerCount, GL_REPEAT, GL_LINEAR
        );

        attachments[COPY_DEPTH_ATTACHMENT] = new Texture2DArray(
            GL_DEPTH_COMPONENT24, 1, windowWidth, windowHeight, layerCount, GL_REPEAT, GL_LINEAR
        );
        glViewport(0, 0, windowWidth, windowHeight);
        frameBuffers[FOCUS_FRAME] = new FrameBuffer();
        frameBuffers[FOCUS_FRAME]->attachFrameBuffer(GL_COLOR_ATTACHMENT0,
            attachments[COLOR_ATTACHMENT]->getID());
        frameBuffers[FOCUS_FRAME]->attachFrameBuffer(GL_DEPTH_ATTACHMENT,
            attachments[DEPTH_ATTACHMENT]->getID());
    }

	void renderer::prepareRenderPassBuffers(const camera* cameraPtr, const light* lightPtr) {
        textures[LAYER_COUNT_TEX] = new Texture(GL_R32I, windowWidth, windowHeight, 
            GL_REPEAT, GL_LINEAR, GL_RED_INTEGER, GL_INT);
        textures[LAYER_COUNT_TEX]->bindImageTexture(1, GL_READ_WRITE, GL_R32I);
        std::vector<std::string> modelsPath;
        switch (SCENE_NUM) {
        case 1:
            objects.push_back(new Object(glm::vec3(0.0f, 0.0f, 0.0f), 
                glm::vec3(0.0f, 90, 0.0f), glm::vec3(1.0f)));
            modelsPath.push_back("../models/bar_plane.obj");
            break;
        case 2:
            objects.push_back(new Object(glm::vec3(0.0f, -0.2f, 2.0f), 
                glm::vec3(-90.0f, 0.0f, 40.0f), glm::vec3(3.0f)));
            modelsPath.push_back("../models/viking_room.obj");
            break;
        case 3:
            objects.push_back(new Object(glm::vec3(0.0f),
                glm::vec3(-7.0f, 0.0f, 0.0f), glm::vec3(25.0f)));
            modelsPath.push_back("../models/untitled2.obj");
            break;
        case 4:
            objects.push_back(new Object(glm::vec3(0.0f), 
                glm::vec3(0.0f, 270, 0.0f), glm::vec3(1.0f)));
            modelsPath.push_back("../models/sponza.obj");
            break;
        }
        
        for (int i = 0; i < objects.size(); i++) {
            objects[i]->loadModel(modelsPath[i]);
            objects[i]->prepareObject();
            objects[i]->setShaderParams(lightPtr, cameraPtr);
            objects[i]->getShader()->setInt("prevDepthmap", 0);
            objects[i]->getShader()->setInt("depthDisc", 1);
            objects[i]->getShader()->setVec2("windowDimension", windowWidth, windowHeight);
            objects[i]->getShader()->setVec2("cameraFarNear", cameraPtr->getFar(), 
                cameraPtr->getNear());
		}
	}

    void renderer::prepareDepthDiscontinuity(const camera* cameraPtr) {
        shaders[DEPTH_DISC_SHADER] = new shader("../src/shaders/depthDisc.compute");
        textures[DEPTH_DISC_TEX] = new Texture(GL_R32F, windowWidth, windowHeight,
            GL_REPEAT, GL_LINEAR, GL_RED, GL_FLOAT);
        textures[DEPTH_DISC_TEX]->bindImageTexture(0, GL_WRITE_ONLY, GL_R32F);
        shaders[DEPTH_DISC_SHADER]->use();
        shaders[DEPTH_DISC_SHADER]->setInt("depthmap", 0);
        shaders[DEPTH_DISC_SHADER]->setVec2("cameraFarNear", cameraPtr->getFar(), cameraPtr->getNear());
        //focal length and focus distance are in meter and aperture is in pixels
        shaders[DEPTH_DISC_SHADER]->setVec3("eyeLens_focusDist_aperture",
            cameraPtr->getEyeLens(), cameraPtr->getFocusDist(), cameraPtr->getAperture());
        shaders[DEPTH_DISC_SHADER]->setInt("coc_max", COC_MAX);
    }

    void renderer::prepareMerging(const camera* cameraPtr) {
        shaders[MERGING_SHADER] = new shader("../src/shaders/mergeFragments.compute");
        
        textures[MERGED_FRAG_COUNT] = new Texture(GL_R32I, 
            ceil(windowWidth / mergeFactor), ceil(windowHeight / mergeFactor),
            GL_REPEAT, GL_LINEAR, GL_RED_INTEGER, GL_INT);
        textures[MERGED_FRAG_COUNT]->bindImageTexture(2, GL_READ_WRITE, GL_R32I);
        
        unsigned int maxFragCount = windowWidth / mergeFactor * windowHeight / mergeFactor
            * MAX_FRAGMENT_COUNT;

        buffers[FRAG_COLOR_DEPTH_BUFFER] = new StorageBuffer(3, 
            maxFragCount * sizeof(glm::vec4), NULL, GL_DYNAMIC_COPY);
        buffers[FRAG_MERGING_DATA_BUFFER] = new StorageBuffer(4,
            maxFragCount * sizeof(glm::uvec4) , NULL, GL_DYNAMIC_COPY);
        buffers[TEST_BUFFER] = new StorageBuffer(8, sizeof(int),
            NULL, GL_DYNAMIC_COPY);

        shaders[MERGING_SHADER]->use();
        shaders[MERGING_SHADER]->setInt("depthmap", 0);
        shaders[MERGING_SHADER]->setInt("colorTexture", 1);
        shaders[MERGING_SHADER]->setVec2("cameraFarNear", cameraPtr->getFar(), cameraPtr->getNear());
        //focal length and focus distance are in meter and aperture is in pixels
        shaders[MERGING_SHADER]->setVec3("eyeLens_focusDist_aperture",
            cameraPtr->getEyeLens(), cameraPtr->getFocusDist(), cameraPtr->getAperture());
    }

    void renderer::prepareSplatting(const camera* cameraPtr) {
        shaders[SPLATTING_SHADER] = new shader("../src/shaders/splat.compute");
        unsigned int tilingSize =
            ceil(windowWidth / tileSize) * ceil(windowHeight / tileSize);
        unsigned int maxSplattedFragCount = tilingSize * MAX_FRAGMENT_TILE;
        
        buffers[SPLATTED_COLOR_DEPTH_BUFFER] = new StorageBuffer(5,
            maxSplattedFragCount * sizeof(glm::vec4), NULL, GL_DYNAMIC_COPY);
        buffers[SPLATTED_FRAG_INFO_BUFFER] = new StorageBuffer(6,
            maxSplattedFragCount * sizeof(glm::uvec4), NULL, GL_DYNAMIC_COPY);
        buffers[TILING_COUNTER_BUFFER] = new StorageBuffer(7, 
            tilingSize * sizeof(unsigned int), NULL, GL_DYNAMIC_COPY);

        shaders[SPLATTING_SHADER]->use();
        shaders[SPLATTING_SHADER]->setVec3("eyeLens_focusDist_aperture",
            cameraPtr->getEyeLens(), cameraPtr->getFocusDist(), cameraPtr->getAperture());
        shaders[SPLATTING_SHADER]->setVec2("tile_size_spread",
            tileSize, tileSpread);
        shaders[SPLATTING_SHADER]->setFloat("coc_max", COC_MAX);
    }

    void renderer::prepareSorting(const camera* cameraPtr) {
        shaders[SORTING_SHADER] = new shader("../src/shaders/sort.compute");
        unsigned int tilingSize =
            ceil(windowWidth / tileSize) * ceil(windowHeight / tileSize);
        unsigned int maxSortedFragCount = tilingSize * MAX_FRAGMENT_TILE;

        buffers[SORTED_COLOR_DEPTH_BUFFER] = new StorageBuffer(9,
            maxSortedFragCount * sizeof(glm::vec4), NULL, GL_DYNAMIC_COPY);
        buffers[SORTED_FRAG_INFO_BUFFER] = new StorageBuffer(10,
            maxSortedFragCount * sizeof(glm::uvec4), NULL, GL_DYNAMIC_COPY);

        shaders[SORTING_SHADER]->use();
        shaders[SORTING_SHADER]->setVec3("eyeLens_focusDist_aperture",
            cameraPtr->getEyeLens(), cameraPtr->getFocusDist(), cameraPtr->getAperture());
        shaders[SORTING_SHADER]->setVec2("cameraFarNear", cameraPtr->getFar(),
            cameraPtr->getNear());
        shaders[SORTING_SHADER]->setVec2("windowDimension", windowWidth, windowHeight);
        shaders[SORTING_SHADER]->setFloat("tileSize", tileSize);
        shaders[SORTING_SHADER]->setFloat("coc_max", COC_MAX);
    }
    
    void renderer::prepareAccumulation() {
        shaders[ACCUMULATION_SHADER] = new shader("../src/shaders/accumulation.compute");

        textures[FINAL_IMAGE] = new Texture(GL_RGBA32F, windowWidth, windowHeight,
            GL_REPEAT, GL_LINEAR, GL_RGBA, GL_FLOAT);
        textures[FINAL_IMAGE]->bindImageTexture(3, GL_READ_WRITE, GL_RGBA32F);

        shaders[ACCUMULATION_SHADER]->use();
        shaders[ACCUMULATION_SHADER]->setFloat("tileSize", tileSize);
    }

    void renderer::prepareScreenQuad() {
        shaders[SCREEN_SHADER] = new shader("../src/shaders/screenShader.vert",
            "../src/shaders/screenShader.frag");
        
        quads[SCREEN_QUAD] = new Quad();
        shaders[SCREEN_SHADER]->use();
        shaders[SCREEN_SHADER]->setInt("screenTexture", 0);
        shaders[SCREEN_SHADER]->setInt("depthTexture", 1);
        shaders[SCREEN_SHADER]->setInt("testTex", 2);
    }
	
	void renderer::renderLoop(const camera* cameraPtr) {
        timer->tick();
        frameBuffers[FOCUS_FRAME]->bind();
        glEnable(GL_DEPTH_TEST);
		glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (int i = 0; i < objects.size(); i++) {
            objects[i]->getShader()->use();
            objects[i]->setShaderCameraParams(cameraPtr);
            attachments[COPY_DEPTH_ATTACHMENT]->bind(0);
            textures[DEPTH_DISC_TEX]->bind(1);
            objects[i]->draw();   
            attachments[COPY_DEPTH_ATTACHMENT]->unbind();
            textures[DEPTH_DISC_TEX]->unbind();
		}
        attachments[DEPTH_ATTACHMENT]->copy(attachments["copyDepthAttachment"]->getID(), 
            GL_TEXTURE_2D_ARRAY, windowWidth, windowHeight, layerCount);
        frameBuffers[FOCUS_FRAME]->unbind();
        glDisable(GL_DEPTH_TEST);
	}

    void renderer::generateDepthDiscMap() {
        shaders[DEPTH_DISC_SHADER]->use();
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        attachments[DEPTH_ATTACHMENT]->bind(0);
        glDispatchCompute(
            ceil(windowWidth / DEPTH_DISC_WORK_GROUP_SIZE),
            ceil(windowHeight / DEPTH_DISC_WORK_GROUP_SIZE),
            1
        );
        attachments[DEPTH_ATTACHMENT]->unbind();
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }

    void renderer::mergeFragments() {
        shaders[MERGING_SHADER]->use();
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
        attachments[DEPTH_ATTACHMENT]->bind(0);
        attachments[COLOR_ATTACHMENT]->bind(1);
        glDispatchCompute(ceil(windowWidth / mergeFactor), 
            ceil(windowHeight / mergeFactor), 1);
        attachments[DEPTH_ATTACHMENT]->unbind();
        attachments[COLOR_ATTACHMENT]->unbind();
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
        //mergeTest();
    }

    void renderer::splatFragments() {
        shaders[SPLATTING_SHADER]->use();
        unsigned int tilingSize =
            ceil(windowWidth / tileSize) * ceil(windowHeight / tileSize);
        std::vector<unsigned int> reset(tilingSize, 0);
        buffers[TILING_COUNTER_BUFFER]->setBufferData(
            tilingSize * sizeof(unsigned int), reset.data());
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
        glDispatchCompute(
            ceil(windowWidth / (mergeFactor * SPLAT_WORK_GROUP_SIZE)),
            ceil(windowHeight / (mergeFactor * SPLAT_WORK_GROUP_SIZE)), 
            1
        );
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
        //splatTest();
    }

    void renderer::sortFragments() {
        shaders[SORTING_SHADER]->use();
        glm::ivec2 tiledImSize = glm::ivec2(ceil(windowWidth / tileSize), 
            ceil(windowHeight / tileSize));
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
        glDispatchCompute(tiledImSize.x, tiledImSize.y, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT); 
        //sortTest(121, 26);
    }

    void renderer::accumulateFragment() {
        shaders[ACCUMULATION_SHADER]->use();
        glm::ivec2 tiledImSize = glm::ivec2(ceil(windowWidth / tileSize),
            ceil(windowHeight / tileSize));
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
        glDispatchCompute(tiledImSize.x, tiledImSize.y, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
        //accumulationTest();
    }
    
    void renderer::storeFrame(bool shouldStore, std::string name) {
        if (shouldStore) {
            textures[FINAL_IMAGE]->saveImagePNG("../samples/" + name, 
                windowWidth, windowHeight, 4);
        }
    }

    void renderer::quadRenderLoop() {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        shaders[SCREEN_SHADER]->use();
        quads[SCREEN_QUAD]->bindVertexArray();
        attachments[COLOR_ATTACHMENT]->bind(0);
        attachments[DEPTH_ATTACHMENT]->bind(1);
        quads[SCREEN_QUAD]->draw();
        attachments[COLOR_ATTACHMENT]->unbind();
        attachments[DEPTH_ATTACHMENT]->unbind();
        quads[SCREEN_QUAD]->unbindVertexArray();
        timer->tock();
        //std::cout << "fps: " << timer->fps() << std::endl;
    }

    void renderer::mergeTest(int coordX, int coordY) {
        buffers[FRAG_MERGING_DATA_BUFFER]->bind();
        glm::uvec4* mergeData = buffers[FRAG_MERGING_DATA_BUFFER]
            ->getBufferData<glm::uvec4>();
        buffers[FRAG_MERGING_DATA_BUFFER]->unbind();

        buffers[FRAG_COLOR_DEPTH_BUFFER]->bind();
        glm::vec4* colorDepth = buffers[FRAG_COLOR_DEPTH_BUFFER]
            ->getBufferData<glm::vec4>();
        buffers[FRAG_COLOR_DEPTH_BUFFER]->unbind();
        buffers[TEST_BUFFER]->bind();
        int y = (windowHeight - 1 - coordY) / mergeFactor , 
            x = coordX / mergeFactor;
        if (once == 9) {
            std::cout << "x: " << x * 2 << " y: " << y * 2 << std::endl;
            for (int el = 0; el < MAX_FRAGMENT_COUNT; el++) {
                int idx = y * windowWidth * 0.5f * MAX_FRAGMENT_COUNT + 
                    x * MAX_FRAGMENT_COUNT + el;
                std::cout << "Color Depth: " << std::endl;
                std::cout << colorDepth[idx].x << " " << colorDepth[idx].y << " " <<
                    colorDepth[idx].z << " " << colorDepth[idx].w << std::endl;
                std::cout << "Merging Data: " << std::endl;
                std::cout << mergeData[idx].x << " " << mergeData[idx].y << " " <<
                    mergeData[idx].z << std::endl;
            }
        }
        once++;
    }
    
    void renderer::splatTest(int coordX, int coordY) {
        buffers[TILING_COUNTER_BUFFER]->bind();
        unsigned int* counter = buffers[TILING_COUNTER_BUFFER]
            ->getBufferData<unsigned int>();
        buffers[TILING_COUNTER_BUFFER]->unbind();
        const int tileWidth = windowWidth / tileSize;
        int y = (windowHeight - 1 - coordY) / tileSize, x = coordX / tileSize;
        int idx = y * tileWidth + x;
        buffers[SPLATTED_COLOR_DEPTH_BUFFER]->bind();
        glm::vec4* colorDepth = buffers[SPLATTED_COLOR_DEPTH_BUFFER]
            ->getBufferData<glm::vec4>();
        buffers[SPLATTED_COLOR_DEPTH_BUFFER]->unbind();

        buffers[SPLATTED_FRAG_INFO_BUFFER]->bind();
        glm::uvec4* fragInfo = buffers[SPLATTED_FRAG_INFO_BUFFER]
            ->getBufferData<glm::uvec4>();
        buffers[SPLATTED_FRAG_INFO_BUFFER]->unbind();

        if (once == 4) {
            std::cout << counter[idx] << std::endl;
            for (int i = 0; i < counter[idx]; i++) {
                int buffIndex = y * tileWidth * MAX_FRAGMENT_TILE + x * MAX_FRAGMENT_TILE + i;
                std::cout << "Color and Depth: " << std::endl;
                std::cout << colorDepth[buffIndex].x << " " << colorDepth[buffIndex].y
                    << " " << colorDepth[buffIndex].z << " " << colorDepth[buffIndex].w
                    << std::endl;
                std::cout << "Frag Info: " << std::endl;
                std::cout << fragInfo[buffIndex].x << " " << fragInfo[buffIndex].y << " "
                    << " " << fragInfo[buffIndex].z << std::endl;
            }
        }
        once++;
    }

    void renderer::sortTest(int coordX, int coordY) {
        buffers[TILING_COUNTER_BUFFER]->bind();
        unsigned int* counter = buffers[TILING_COUNTER_BUFFER]
            ->getBufferData<unsigned int>();
        buffers[TILING_COUNTER_BUFFER]->unbind();

        buffers[SPLATTED_COLOR_DEPTH_BUFFER]->bind();
        glm::vec4* splatDepCol = buffers[SPLATTED_COLOR_DEPTH_BUFFER]
            ->getBufferData<glm::vec4>();
        buffers[SPLATTED_COLOR_DEPTH_BUFFER]->unbind();

        buffers[SORTED_COLOR_DEPTH_BUFFER]->bind();
        glm::vec4* colorDepth = buffers[SORTED_COLOR_DEPTH_BUFFER]
            ->getBufferData<glm::vec4>();
        buffers[SORTED_COLOR_DEPTH_BUFFER]->unbind();
        
        buffers[SORTED_FRAG_INFO_BUFFER]->bind();
        glm::uvec4* fragInfo = buffers[SORTED_FRAG_INFO_BUFFER]
            ->getBufferData<glm::uvec4>();
        buffers[SORTED_FRAG_INFO_BUFFER]->unbind();
        int tileWidth = ceil(windowWidth / tileSize);
        int y = (windowHeight - 1 - coordY) / tileSize, x = coordX / tileSize;
        int idx = y * tileWidth + x;
        if (once >= 4 && once <= 10) {
            std::cout << "_____________________________________" << std::endl;
            std::cout << counter[idx] << std::endl;
            for (int i = 0; i < 1; i++) {
                int buffIdx = y * tileWidth * MAX_FRAGMENT_TILE + x
                    * MAX_FRAGMENT_TILE + i;
                std::cout << "Fragment: " << i << std::endl;
                std::cout << colorDepth[buffIdx].r << " " << colorDepth[buffIdx].g
                    << " " << colorDepth[buffIdx].b << " " << colorDepth[buffIdx].a
                    << std::endl;
                std::cout  << fragInfo[buffIdx].x / FLOATING_PERCISION
                    << " " << fragInfo[buffIdx].y / FLOATING_PERCISION
                    << " " << fragInfo[buffIdx].z / FLOATING_PERCISION 
                    << " " << fragInfo[buffIdx].w / FLOATING_PERCISION 
                    << std::endl;
            }
        }
        once++;
    }

    void renderer::accumulationTest() {
        buffers[TEST_BUFFER]->bind();
        int* test = buffers[TEST_BUFFER]->getBufferData<int>();
        buffers[TEST_BUFFER]->unbind();
        std::cout << test[0] << std::endl;
    }
    
    void renderer::releaseMemory() {
        for (auto frameBuffer : frameBuffers) {
            frameBuffer.second->deleteFrameBuffer();
        }
        for (auto attachment : attachments) {
            attachment.second->deleteTexture();
        }
        for (auto texture : textures) {
            texture.second->deleteTexture();
        }
        for (auto buffer : buffers) {
            buffer.second->deleteBuffer();
        }
        for (auto quad : quads) {
            quad.second->releaseMemory();
        }
        for (int i = 0; i < objects.size(); i++) {
            objects[i]->deleteBuffer();
		}
	}

}