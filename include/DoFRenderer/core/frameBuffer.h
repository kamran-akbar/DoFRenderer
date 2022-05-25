#pragma once
#include "glad/glad.h"

namespace DoFRenderer {
	class FrameBuffer {
	public:
		FrameBuffer();
		~FrameBuffer() { }

		void attachFrameBuffer(unsigned int attachmetType, unsigned int attachmentID);
		void attachFrameBuffer(unsigned int attachmetType, unsigned int textureType, unsigned int attachmentID);
		unsigned int checkFrameBufferStatus();
		void copyFramebuffer(unsigned int destFramebufferID, unsigned int width, unsigned int height);
		void bind();
		void unbind();
		void deleteFrameBuffer();
		inline unsigned int getID() const { return id; }
	private:
		unsigned int id;
	};
}