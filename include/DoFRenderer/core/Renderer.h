#pragma once
#include "DoFRenderer/core/object.h"
#include "DoFRenderer/core/camera.h"
#include "DoFRenderer/core/shader.h"
#include "DoFRenderer/core/light.h"
#include "DoFRenderer/core/time.h"
#include "DoFRenderer/core/Texture2DArray.h"
#include "DoFRenderer/core/Texture.h"
#include <iostream>
#include <unordered_map>

namespace DoFRenderer {
	

	class renderer {
	
	public:
		renderer();
		renderer(unsigned int width, unsigned int height, unsigned int layerCount);
		~renderer();
		void generateFrameBuffers();
		void prepareRenderPassBuffers(const camera* cameraPtr, const light* lightPtr);
		void prepareDepthDiscontinuity();
		void prepareScreenQuad();
		void renderLoop();
		void generateDepthDiscMap();
		void quadRenderLoop();
		void deleteBuffers();
	private:
		std::vector<Object*> objects;
		Timer* timer;
		std::unordered_map<std::string, Texture2DArray*> attachments;
		std::unordered_map<std::string, shader*> shaders;
		std::unordered_map<std::string, Texture*> textures;
		unsigned int frameBuffer;
		unsigned int quadVertexArray;
		unsigned int quadVertexBuffer;
		unsigned int windowWidth, windowHeight;
		unsigned int layerCount;
		bool once = false;

	};
}