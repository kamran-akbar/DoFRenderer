#pragma once

namespace DoFRenderer {
	class Texture2DArray {
	public:
		Texture2DArray() { }
		Texture2DArray(
			unsigned int format, unsigned int mipmapLevels, unsigned int windowWidth,
			unsigned int windowHeight, unsigned int layerCount, unsigned int wrapMode, 
			unsigned int filterMode
		);
		~Texture2DArray() { }

		void bind(int i);
		void copy(const unsigned int destID, unsigned int destTarget, unsigned int width,
			unsigned int height, unsigned int layerCount);
		void unbind();
		void deleteTexture();
		

		inline unsigned int getID() const { return id; }

	private:
		unsigned int id;
	};
}