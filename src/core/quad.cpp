#include "DoFRenderer/core/quad.h"
#include <vector>

namespace DoFRenderer {
	Quad::Quad() {
        std::vector<glm::vec4> quadVertices = {
            glm::vec4(-1.0f, 1.0f, 0.0f, 1.0f), glm::vec4(-1.0f, -1.0f, 0.0f, 0.0f),
            glm::vec4(1.0f, -1.0f, 1.0f, 0.0f), glm::vec4(-1.0f, 1.0f, 0.0f, 1.0f),
            glm::vec4(1.0f, -1.0f, 1.0f, 0.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
        };

        glGenVertexArrays(1, &vertexArrayID);
        glGenBuffers(1, &vertexBufferID);
        glBindVertexArray(vertexArrayID);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
        glBufferData(GL_ARRAY_BUFFER, quadVertices.size() * sizeof(glm::vec4),
            quadVertices.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)(sizeof(glm::vec2)));
	}

    void Quad::bindVertexArray() {
        glBindVertexArray(vertexArrayID);
    }

    void Quad::unbindVertexArray() {
        glBindVertexArray(0);
    }

    void Quad::draw() {
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    void Quad::releaseMemory() {
        glDeleteVertexArrays(1, &vertexArrayID);
        glDeleteBuffers(1, &vertexBufferID);
    }
}