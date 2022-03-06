#pragma once
#include "DoFRenderer/core/object.h"
#include "DoFRenderer/core/camera.h"
#include "DoFRenderer/core/shader.h"
#include "DoFRenderer/core/light.h"
#include "DoFRenderer/core/time.h"
#include "DoFRenderer/core/frameBuffer.h"
#include "DoFRenderer/core/Texture2DArray.h"
#include "DoFRenderer/core/Texture.h"
#include "DoFRenderer/core/storageBuffer.h"
#include "DoFRenderer/core/fragmentData.h"
#include "DoFRenderer/core/quad.h"
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
		void prepareDepthDiscontinuity(const camera* cameraPtr);
		void prepareMerging(const camera* cameraPtr);
		void prepareSplatting(const camera* cameraPtr);
		void prepareSorting(const camera* cameraPtr);
		void prepareAccumulation();
		void prepareScreenQuad();
		void renderLoop(const camera* cameraPtr);
		void generateDepthDiscMap();
		void mergeFragments();
		void splatFragments();
		void sortFragments();
		void accumulateFragment();
		void storeFrame(bool shouldStore, std::string name);
		void quadRenderLoop();
		void releaseMemory();

		void mergeTest(int coordX, int coordY);
		void splatTest(int coordX, int coordY);
		void sortTest(int coordX, int coordY);
		void accumulationTest();
	private:
		std::vector<Object*> objects;
		Timer* timer;
		std::unordered_map<std::string, Texture2DArray*> attachments;
		std::unordered_map<std::string, shader*> shaders;
		std::unordered_map<std::string, Texture*> textures;
		std::unordered_map<std::string, StorageBuffer*> buffers;
		std::unordered_map<std::string, FrameBuffer*> frameBuffers;
		std::unordered_map<std::string, Quad*> quads;
		unsigned int windowWidth, windowHeight;
		unsigned int layerCount;
		int once = 0;

		static const int tileSize = 16;
		static const int tileSpread = 1;
		static const int mergeFactor = 2;
	};
}