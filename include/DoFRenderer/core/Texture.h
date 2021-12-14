#pragma once
#include <string>
#include <iostream>

namespace DoFRenderer {
	
	class Texture {
	public:
		Texture() : texture(-1) {}
		Texture(std::string name, std::string filepath, unsigned int wrapMode, 
			unsigned int filterMode, unsigned int format, unsigned int internalFormat, 
			unsigned int type);

		void bind(unsigned short i);
		void unbind();
		void deleteTexture();

		inline std::string getName() { return name; }

	private:
		unsigned int texture;
		std::string name;
	};
}