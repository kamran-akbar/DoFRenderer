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

	void FrameBuffer::attachFrameBuffer(unsigned int attachmetType, unsigned int textureType, unsigned int attachmentID) {
		bind();
		glFramebufferTexture3D(GL_FRAMEBUFFER, attachmetType, textureType, attachmentID, 0, 0);
		if (checkFrameBufferStatus() != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Error::Framebuffer is not compelete::"
			<< "issue with attachement type: " << attachmetType
			<< " with attachment id: " << attachmentID << std::endl;
		unbind();
	}

	unsigned int FrameBuffer::checkFrameBufferStatus() {
		return glCheckFramebufferStatus(GL_FRAMEBUFFER);
	}

	void FrameBuffer::copyFramebuffer(unsigned int destFramebufferID, unsigned int width, unsigned int height) {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, id);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, destFramebufferID);	
		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
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