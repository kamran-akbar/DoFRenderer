#include "DoFRenderer/core/Renderer.h"
#include "DoFRenderer/core/mesh.h"

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
        for (Object* obj : objects) {
            delete obj;
        }
    }

    void renderer::generateLayeredFrameBuffer() {

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
            objects[i]->getShader()->setVec2("windowDimension", windowWidth, windowHeight);
            objects[i]->getShader()->setVec2("cameraFarNear", cameraPtr->getFar(), cameraPtr->getNear());
		}
	}

    void renderer::prepareScreenQuad() {
        screenShader = new shader("../src/shaders/screenShader.vert", 
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
        screenShader->setInt("screenTexture", 0);
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
            objects[i]->draw();   
            attachments["copyDepthAttachment"]->unbind();
		}
        attachments["depthAttachment"]->copy(attachments["copyDepthAttachment"]->getID(), 
            GL_TEXTURE_2D_ARRAY, windowWidth, windowHeight, layerCount);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);
	}

    void renderer::quadRenderLoop() {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        screenShader->use();
        glBindVertexArray(quadVertexArray);
        attachments["colorAttachment"]->bind(0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        attachments["colorAttachment"]->unbind();
        timer->tock();
        //std::cout << "fps: " << timer->fps() << std::endl;
    }

	void renderer::deleteBuffers() {
        glDeleteFramebuffers(1, &frameBuffer);
        attachments["colorAttachment"]->deleteTexture();
        attachments["depthAttachment"]->deleteTexture();
        attachments["copyDepthAttachment"]->deleteTexture();
        glDeleteVertexArrays(1, &quadVertexArray);
        glDeleteBuffers(1, &quadVertexBuffer);

        for (int i = 0; i < objects.size(); i++) {
            objects[i]->deleteBuffer();
		}
	}
    
}