#include "DoFRenderer/core/Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
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
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            throw "Failed to load texture";
        }
        stbi_image_free(data);
	}

    void Texture::bind(unsigned short i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, texture);
    }

    void Texture::unbind() {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::deleteTexture() {
        glDeleteTextures(1, &texture);
    }
}