#include "DoFRenderer/core/storageBuffer.h"
#include "glad/glad.h"
#include <iostream>
namespace DoFRenderer {
	StorageBuffer::StorageBuffer(unsigned int binding, unsigned int dataSize,
		const void* data, unsigned int bufferUsage) {
		glGenBuffers(1, &id);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, id);
		glBufferData(GL_SHADER_STORAGE_BUFFER, dataSize, data, bufferUsage);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	void StorageBuffer::bind() {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
	}

	void StorageBuffer::bindBufferBase(unsigned int binding) {
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, id);
	}

	void StorageBuffer::setBufferData(unsigned int dataSize, const void* data) {
		glNamedBufferSubData(id, 0, dataSize, data);
	}

	void StorageBuffer::unbind() {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	void* StorageBuffer::getBufferData() const {
		void* data = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		return data;
	}

	void StorageBuffer::deleteBuffer() {
		glDeleteBuffers(1, &id);
	}
}