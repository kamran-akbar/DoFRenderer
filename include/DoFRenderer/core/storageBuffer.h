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
		void* getBufferData() const;
		void deleteBuffer();
		inline unsigned int getID() const { return id; }

	private:
		unsigned int id;
	};
}