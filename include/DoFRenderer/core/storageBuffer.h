#pragma once

namespace DoFRenderer {
	class StorageBuffer {
	public:
		StorageBuffer() { }
		
		StorageBuffer(unsigned int binding, unsigned int dataSize,
			const void* data, unsigned int bufferUsage);
		
		~StorageBuffer() { }
		
		void bind();
		
		void bindBufferBase(unsigned int binding);
		
		void setBufferData(unsigned int dataSize, const void* data);
		
		void unbind();

		template<typename T> 
		inline T* getBufferData() const {
			void* data = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
			glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
			return (T*)data;
		}

		void deleteBuffer();
		
		inline unsigned int getID() const { return id; }

	private:
		unsigned int id;
	};
}