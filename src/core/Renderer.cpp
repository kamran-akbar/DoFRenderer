#include "DoFRenderer/core/Renderer.h"
#include "DoFRenderer/core/mesh.h"

#define WORK_GROUP_SIZE 32

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
        for (Object* obj : objects) {
            delete obj;
        }
    }

    void renderer::generateFrameBuffers() {

        attachments["colorAttachment"] = new Texture2DArray(
            GL_RGBA8, 1, windowWidth, windowHeight, layerCount, GL_REPEAT, GL_LINEAR
        );
        
        attachments["depthAttachment"] = new Texture2DArray(
            GL_DEPTH_COMPONENT24, 1, windowWidth, windowHeight, layerCount, GL_REPEAT, GL_LINEAR
        );

        attachments["copyDepthAttachment"] = new Texture2DArray(
            GL_DEPTH_COMPONENT24, 1, windowWidth, windowHeight, layerCount, GL_REPEAT, GL_LINEAR
        );

        glGenFramebuffers(1, &frameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, attachments["colorAttachment"]->getID(), 0);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, attachments["depthAttachment"]->getID(), 0);
        
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
    }

	void renderer::prepareRenderPassBuffers(const camera* cameraPtr, const light* lightPtr) {
        
        objects.push_back(new Object(glm::vec3(0.0f, -0.2f, 0.0f), glm::vec3(-90.0f, 0.0f, 40.0f), glm::vec3(3.0f)));
        //objects.push_back(new Object(glm::vec3(0.0f), glm::vec3(-15.0f, 0.0f, 0.0f), glm::vec3(25.0f)));

        std::vector<std::string> modelsPath = {
            "../models/viking_room.obj"
        };

        for (int i = 0; i < objects.size(); i++) {
            objects[i]->loadModel(modelsPath[i]);
            objects[i]->prepareObject();
            objects[i]->setShaderParams(lightPtr, cameraPtr);
            objects[i]->getShader()->setInt("prevDepthmap", 0);
            objects[i]->getShader()->setInt("depthDiscTex", 1);
            objects[i]->getShader()->setVec2("windowDimension", windowWidth, windowHeight);
            objects[i]->getShader()->setVec2("cameraFarNear", cameraPtr->getFar(), cameraPtr->getNear());
		}
	}

    void renderer::prepareDepthDiscontinuity() {
        shaders["depthDiscShader"] = new shader("../src/shaders/depthDisc.compute");
        textures["depthDiscTex"] = new Texture(GL_R32F, windowWidth, windowHeight,
            GL_REPEAT, GL_LINEAR, GL_RED, GL_FLOAT);
        textures["depthDiscTex"]->bindImageTexture(1, GL_WRITE_ONLY, GL_R32F);
        shaders["depthDiscShader"]->use();
        shaders["depthDiscShader"]->setInt("depthmap", 0);
    }

    void renderer::prepareScreenQuad() {
        shaders["screenShader"] = new shader("../src/shaders/screenShader.vert",
            "../src/shaders/screenShader.frag");
        
        std::vector<glm::vec4> quadVertices = {
            glm::vec4(-1.0f, 1.0f, 0.0f, 1.0f), glm::vec4(-1.0f, -1.0f, 0.0f, 0.0f),
            glm::vec4(1.0f, -1.0f, 1.0f, 0.0f), glm::vec4(-1.0f, 1.0f, 0.0f, 1.0f),
            glm::vec4(1.0f, -1.0f, 1.0f, 0.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
        };

        glGenVertexArrays(1, &quadVertexArray);
        glGenBuffers(1, &quadVertexBuffer);
        glBindVertexArray(quadVertexArray);
        glBindBuffer(GL_ARRAY_BUFFER, quadVertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, quadVertices.size() * sizeof(glm::vec4),
            quadVertices.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)(sizeof(glm::vec2)));
        shaders["screenShader"]->use();
        shaders["screenShader"]->setInt("screenTexture", 0);
        shaders["screenShader"]->setInt("depthTexture", 1);
        shaders["screenShader"]->setInt("testTex", 2);
    }
	
	void renderer::renderLoop() {
        timer->tick();
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
        glEnable(GL_DEPTH_TEST);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (int i = 0; i < objects.size(); i++) {
            objects[i]->getShader()->use();
            attachments["copyDepthAttachment"]->bind(0);
            textures["depthDiscTex"]->bind(1);
            objects[i]->draw();   
            attachments["copyDepthAttachment"]->unbind();
            textures["depthDiscTex"]->unbind();
		}
        attachments["depthAttachment"]->copy(attachments["copyDepthAttachment"]->getID(), 
            GL_TEXTURE_2D_ARRAY, windowWidth, windowHeight, layerCount);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);
	}

    void renderer::generateDepthDiscMap() {
        shaders["depthDiscShader"]->use();
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        attachments["depthAttachment"]->bind(0);
        glDispatchCompute(windowWidth / WORK_GROUP_SIZE, windowHeight / WORK_GROUP_SIZE, 1);
        attachments["depthAttachment"]->unbind();
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }

    void renderer::quadRenderLoop() {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        shaders["screenShader"]->use();
        glBindVertexArray(quadVertexArray);
        attachments["colorAttachment"]->bind(0);
        attachments["depthAttachment"]->bind(1);
        textures["depthDiscTex"]->bind(2);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        attachments["colorAttachment"]->unbind();
        attachments["depthAttachment"]->unbind();
        textures["depthDiscTex"]->unbind();
        glBindVertexArray(0);
        timer->tock();
        //std::cout << "fps: " << timer->fps() << std::endl;
    }

	void renderer::deleteBuffers() {
        glDeleteFramebuffers(1, &frameBuffer);
        for (auto attachment : attachments) {
            attachment.second->deleteTexture();
        }

        for (auto texture : textures) {
            texture.second->deleteTexture();
        }

        glDeleteVertexArrays(1, &quadVertexArray);
        glDeleteBuffers(1, &quadVertexBuffer);

        for (int i = 0; i < objects.size(); i++) {
            objects[i]->deleteBuffer();
		}
	}
    
}