#include "DoFRenderer/core/Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#include "glad/glad.h"

namespace DoFRenderer {
	
	Texture::Texture(std::string name, std::string filePath, unsigned int wrapMode,
        unsigned int filterMode, unsigned int format, unsigned int internalFormat, 
        unsigned int type) {

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode);
        
        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true); 
       
        unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);
        if (nrChannels == 3) {
            internalFormat = GL_RGB;
            format = GL_RGB;
        }
        else if (nrChannels == 4) {
            internalFormat = GL_RGBA;
            format = GL_RGBA;
        }
        if (data) {
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else {
            throw "Failed to load texture";
        }
        stbi_image_free(data);
	}

    Texture::Texture(
        unsigned int format, unsigned int windowWidth, unsigned int windowHeight,
        unsigned int wrapMode, unsigned int filterMode, unsigned int internalFormat,
        unsigned int type) {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode);
        glTexImage2D(GL_TEXTURE_2D, 0, format, windowWidth, windowHeight, 0, internalFormat,
            type, NULL);
    }

    void Texture::bind(unsigned short i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, texture);
    }

    void Texture::bindImageTexture(unsigned int binding, unsigned int accessMode, 
        unsigned int format) {
        glBindImageTexture(binding, texture, 0, GL_FALSE, 0, accessMode, format);
    }

    void Texture::getTextureImage(unsigned int format, unsigned int type,
        unsigned int level, unsigned int bufferSize, void* data) {
        glGetTextureImage(texture, level, format, type, bufferSize, data);
    }

    void Texture::saveImagePNG(std::string filename, unsigned int width,
        unsigned int height, unsigned int channel) {
        unsigned int buffSize = width * height * channel * sizeof(unsigned char);
        unsigned char* pixels = new unsigned char[buffSize / sizeof(unsigned char)];
        getTextureImage(GL_RGBA, GL_UNSIGNED_BYTE, 0, buffSize, pixels);
        stbi_flip_vertically_on_write(true);
        stbi_write_png(filename.c_str(), width, height, channel, pixels, 
            width * channel);
        stbi_image_free(pixels);
    }

    void Texture::unbind() {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::deleteTexture() {
        glDeleteTextures(1, &texture);
    }
}