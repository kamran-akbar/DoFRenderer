#pragma once
#include <string>
#include <iostream>

namespace DoFRenderer {
	
	class Texture {
	public:
		Texture() : texture(-1) {}
		Texture(
			std::string name, std::string filepath, unsigned int wrapMode, 
			unsigned int filterMode, unsigned int format, unsigned int internalFormat, 
			unsigned int type
		);
		Texture(
			unsigned int format, unsigned int windowWidth, unsigned int windowHeight, 
			unsigned int wrapMode, unsigned int filterMode, unsigned int internalFormat,
			unsigned int type
		);

		void bind(unsigned short i);
		void bindImageTexture(unsigned int binding, unsigned int accessMode, unsigned int format);
		void unbind();
		void deleteTexture();

		inline std::string getName() { return name; }
		inline unsigned int getID() const { return texture; }

	private:
		unsigned int texture;
		std::string name;
	};
}