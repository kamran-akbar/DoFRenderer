#pragma once
#include "glad/glad.h"
#include <string>
#include <iostream>

namespace DoFRenderer {
	
	class Texture {
	public:
		Texture() : texture(-1) {}
		Texture(std::string filepath, unsigned int wrapMode, unsigned int filterMode, unsigned int format, 
			unsigned int internalFormat, unsigned int type);

		void bind(unsigned short i);

	private:
		unsigned int texture;
	};
}