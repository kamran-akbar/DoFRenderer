#include "DoFRenderer/core/Renderer.h"
#include "DoFRenderer/core/mesh.h"

namespace DoFRenderer {

    renderer::renderer() {
        timer = new Timer();
    }
    
    renderer::~renderer() {
        for (Object* obj : objects) {
            delete obj;
        }
    }

	void renderer::prepareRenderPassBuffers(const camera* cameraPtr, const light* lightPtr) {
        glEnable(GL_DEPTH_TEST);
        
        objects.push_back(new Object(glm::vec3(0.0f), glm::vec3(-15.0f, 0.0f, 0.0f), glm::vec3(25.0f)));
   
        //objects.push_back(new Object(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 0.0f)));

        std::vector<std::string> modelsPath = {
            "../models/untitled.obj"
        };

        for (int i = 0; i < objects.size(); i++) {
            objects[i]->loadModel(modelsPath[i]);
            objects[i]->prepareObject();
            objects[i]->setShaderParams(lightPtr, cameraPtr);
		}
	}
	
	void renderer::renderLoop() {
        
        timer->tick();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (int i = 0; i < objects.size(); i++) {
            for (int j = 0; j < objects[i]->getMeshes().size(); j++) {
                objects[i]->getMeshes()[j]->getShader()->use();
                objects[i]->getMeshes()[j]->getShader()->setMat4("model", 
                    objects[i]->getTranformation());
            }
            objects[i]->draw();
		}
        timer->tock();

        std::cout << "fps: " << timer->fps() << "s\n";
	}

	void renderer::deleteBuffers() {
        for (int i = 0; i < objects.size(); i++) {
            objects[i]->deleteBuffer();
		}
	}
    
}