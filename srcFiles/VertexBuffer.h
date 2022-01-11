#pragma once

#include "GLHandles.h"

#include <GL/glew.h>


class VertexBuffer {

public:
	VertexBuffer(GLuint index, GLint size, GLenum dataType);

	// Public interface
	void bind() const { glBindBuffer(GL_ARRAY_BUFFER, bufferID); }
	void uploadData(GLsizeiptr size, const void* data, GLenum usage);

private:
	VertexBufferHandle bufferID;
};

class IndexBuffer {

public:
	IndexBuffer(GLuint index, GLint size, GLenum dataType);

	// Public interface
	void bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID); }
	void uploadData(GLsizeiptr size, const void* data, GLenum usage);

private:
	VertexBufferHandle bufferID;
};

