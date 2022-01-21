#pragma once

#include "GLHandles.h"

#include <GL/glew.h>

#include <utility>


class VertexBuffer {

public:
	VertexBuffer(GLuint index, GLint size, GLenum dataType);

	// Public interface
	void bind() const { glBindBuffer(GL_ARRAY_BUFFER, bufferID); }
	void uploadData(GLsizeiptr size, const void* data, GLenum usage);

private:
	BufferHandle bufferID;
};

class IndexBuffer {

public:
	IndexBuffer(GLuint index, GLint size, GLenum dataType);

	// Public interface
	void bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID); }
	void uploadData(GLsizeiptr attachment, const void* data, GLenum usage);

private:
	BufferHandle bufferID;
};

class FrameBuffer {

public:
	FrameBuffer(GLuint index);

	// Public interface
	void bind() const { glBindFramebuffer(GL_FRAMEBUFFER, bufferID); }
	void unbind() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
	void attachTexture(GLenum attachment, GLuint texture, GLenum buf);

private:
	FrameBufferHandle bufferID;
};
