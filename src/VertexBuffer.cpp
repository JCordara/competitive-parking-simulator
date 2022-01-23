#include "VertexBuffer.h"


VertexBuffer::VertexBuffer(GLuint index, GLint size, GLenum dataType)
	: bufferID{}
{
	bind();
	glVertexAttribPointer(index, size, dataType, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(index);
}


void VertexBuffer::uploadData(GLsizeiptr size, const void* data, GLenum usage) {
	bind();
	glBufferData(GL_ARRAY_BUFFER, size, data, usage);
}

IndexBuffer::IndexBuffer(GLuint index, GLint size, GLenum dataType)
	: bufferID{}
{
	bind();
	glVertexAttribPointer(index, size, dataType, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(index);
}


void IndexBuffer::uploadData(GLsizeiptr size, const void* data, GLenum usage) {
	bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);
}

FrameBuffer::FrameBuffer() : bufferID{}
{
// I dont think anything needs to be done??	
}

void FrameBuffer::attachTexture(GLenum attachment, GLuint texture, GLenum buf) {
	bind();
	glFramebufferTexture(GL_FRAMEBUFFER, attachment, texture, 0);
	glDrawBuffer(buf);
	unbind();
}
