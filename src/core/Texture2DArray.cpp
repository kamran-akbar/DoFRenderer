#include "DoFRenderer/core/Texture2DArray.h"
#include "glad/glad.h"
#include <iostream>

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