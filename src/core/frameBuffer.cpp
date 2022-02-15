#include "DoFRenderer/core/frameBuffer.h"
#include <iostream>

namespace DoFRenderer {
	FrameBuffer::FrameBuffer() {
		glGenFramebuffers(1, &id);
	}

	void FrameBuffer::attachFrameBuffer(unsigned int attachmetType, unsigned int attachmentID) {
		bind();
		glFramebufferTexture(GL_FRAMEBUFFER, attachmetType, attachmentID, 0);
		if (checkFrameBufferStatus() != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Error::Framebuffer is not compelete::" 
			<< "issue with attachement type: " << attachmetType 
			<< "with attachment id: " << attachmentID << std::endl;
		unbind();
	}

	unsigned int FrameBuffer::checkFrameBufferStatus() {
		return glCheckFramebufferStatus(GL_FRAMEBUFFER);
	}

	void FrameBuffer::bind() {
		glBindFramebuffer(GL_FRAMEBUFFER, id);
	}

	void FrameBuffer::unbind() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBuffer::deleteFrameBuffer() {
		glDeleteFramebuffers(1, &id);
	}
}