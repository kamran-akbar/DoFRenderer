#pragma once
#include "glad/glad.h"

namespace DoFRenderer {
	class FrameBuffer {
	public:
		FrameBuffer();
		~FrameBuffer() { }

		void attachFrameBuffer(unsigned int attachmetType, unsigned int attachmentID);
		unsigned int checkFrameBufferStatus();
		void bind();
		void unbind();
		void deleteFrameBuffer();
	private:
		unsigned int id;
	};
}