#pragma once
#include <string>

namespace DoFRenderer {
	class Texture2DArray {
	public:
		Texture2DArray() { }
		Texture2DArray(
			unsigned int format, unsigned int mipmapLevels, unsigned int windowWidth,
			unsigned int windowHeight, unsigned int layerCount, unsigned int wrapMode, 
			unsigned int filterMode
		);
		Texture2DArray(
			unsigned int format, unsigned int mipmapLevels, unsigned int windowWidth,
			unsigned int windowHeight, unsigned int layerCount, unsigned int wrapMode,
			unsigned int filterMode, unsigned int samples
		);
		~Texture2DArray() { }

		void bind(int i, bool sampled);
		void bindImageTexture(unsigned int binding, unsigned int accessMode, unsigned int format);
		void getTextureImage(unsigned int format, unsigned int type,unsigned int level,
			unsigned int bufferSize, void* data);
		void saveImagesPNG(std::string prefix, std::string suffix, unsigned int width, unsigned int height,
			unsigned int channel, unsigned int frames);
		void copy(const unsigned int destID, unsigned int destTarget, unsigned int width,
			unsigned int height, unsigned int layerCount, bool sampled);
		void unbind(bool sampled);
		void deleteTexture();
		

		inline unsigned int getID() const { return id; }

	private:
		unsigned int id;
	};
}