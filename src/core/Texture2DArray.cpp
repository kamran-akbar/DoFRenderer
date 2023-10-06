#include "DoFRenderer/core/Texture2DArray.h"
#include "glad/glad.h"
#include <iostream>
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

namespace DoFRenderer {

	Texture2DArray::Texture2DArray(
		unsigned int format, unsigned int mipmapLevels, unsigned int windowWidth,
		unsigned int windowHeight, unsigned int layerCount, unsigned int wrapMode,
		unsigned int filterMode) {
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D_ARRAY, id);
		glTexStorage3D(GL_TEXTURE_2D_ARRAY, mipmapLevels, format, windowWidth, windowHeight, layerCount);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, wrapMode);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, wrapMode);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, filterMode);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, filterMode);
	}

	Texture2DArray::Texture2DArray(unsigned int format, unsigned int mipmapLevels, unsigned int windowWidth,
		unsigned int windowHeight, unsigned int layerCount, unsigned int wrapMode,
		unsigned int filterMode, unsigned int samples) {
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, id);
		glTexImage3DMultisample(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, samples, format,
			windowWidth, windowHeight, layerCount, GL_TRUE);
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, GL_TEXTURE_WRAP_S, wrapMode);
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, GL_TEXTURE_WRAP_T, wrapMode);
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, GL_TEXTURE_MIN_FILTER, filterMode);
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, GL_TEXTURE_MAG_FILTER, filterMode);
	}

	void Texture2DArray::bind(int i, bool sampled) {
		glActiveTexture(GL_TEXTURE0 + i);
		if(!sampled) glBindTexture(GL_TEXTURE_2D_ARRAY, id);
		else glBindTexture(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, id);
	}

	void Texture2DArray::bindImageTexture(unsigned int binding, unsigned int accessMode, unsigned int format) {
		glBindImageTexture(binding, id, 0, GL_TRUE, 0, accessMode, format);
	}

	void Texture2DArray::getTextureImage(unsigned int format, unsigned int type, unsigned int level,
		unsigned int bufferSize, void* data) {
		glGetTextureImage(id, level, format, type, bufferSize, data);
	}

	void Texture2DArray::saveImagesPNG(std::string prefix, std::string suffix, unsigned int width, unsigned int height,
		unsigned int channel, unsigned int frames) {
		unsigned int buffSize = frames * width * height * channel * sizeof(unsigned char);
		unsigned char* pixel_frames = new unsigned char[buffSize / sizeof(unsigned char)];
		unsigned char* pixel_frame = new unsigned char[buffSize / (sizeof(unsigned char) * frames)];
		getTextureImage(GL_RGBA, GL_UNSIGNED_BYTE, 0, buffSize, pixel_frames);
		for (int i = 0; i < frames; i++) {
			stbi_flip_vertically_on_write(true);
			std::string filename = prefix + std::to_string(i / 3) + std::to_string(i % 3) + suffix;
			std::memcpy(&pixel_frame[0], &pixel_frames[unsigned int(i * width * height * channel)], unsigned int(width * height * channel)
				* sizeof(unsigned char));
			stbi_write_png(filename.c_str(), width, height, channel, pixel_frame, width * channel);
		}
		stbi_image_free(pixel_frames);
		stbi_image_free(pixel_frame);
	}

	void Texture2DArray::copy(const unsigned int destID, unsigned int destTarget,
		unsigned int width, unsigned int height, unsigned int layerCount, bool sampled) {
		if(!sampled)
			glCopyImageSubData(id, GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, destID, destTarget, 0, 0, 0, 0, 
				width, height, layerCount);
		else
			glCopyImageSubData(id, GL_TEXTURE_2D_MULTISAMPLE_ARRAY, 0, 0, 0, 0, destID, destTarget, 0, 0, 0, 0,
				width, height, layerCount);
	}

	void Texture2DArray::unbind(bool sampled) {
		if(!sampled) glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
		else glBindTexture(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, 0);
	}

	void Texture2DArray::deleteTexture() {
		glDeleteTextures(1, &id);
	}

}