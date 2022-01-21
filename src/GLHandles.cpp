#include "GLHandles.h"

#include <algorithm> // For std::swap

ShaderHandle::ShaderHandle(GLenum type)
	: shaderID(glCreateShader(type))
{
}


ShaderHandle::ShaderHandle(ShaderHandle&& other) noexcept
	: shaderID(std::move(other.shaderID))
{
	other.shaderID = 0;
}


ShaderHandle& ShaderHandle::operator=(ShaderHandle&& other) noexcept {
	// Note that swap is implemented with move internally
	std::swap(shaderID, other.shaderID);
	return *this;
}


ShaderHandle::~ShaderHandle() {
	glDeleteShader(shaderID);
}


ShaderHandle::operator GLuint() const {
	return shaderID;
}

GLuint ShaderHandle::value() const {
	return shaderID;
}


//------------------------------------------------------------------------------


ShaderProgramHandle::ShaderProgramHandle()
	: programID(glCreateProgram())
{ }


ShaderProgramHandle::ShaderProgramHandle(ShaderProgramHandle&& other) noexcept
	: programID(std::move(other.programID))
{
	other.programID = 0;
}


ShaderProgramHandle& ShaderProgramHandle::operator=(ShaderProgramHandle&& other) noexcept {
	std::swap(programID, other.programID);
	return *this;
}


ShaderProgramHandle::~ShaderProgramHandle() {
	glDeleteProgram(programID);
}


ShaderProgramHandle::operator GLuint() const {
	return programID;
}


GLuint ShaderProgramHandle::value() const {
	return programID;
}


//------------------------------------------------------------------------------


BufferHandle::BufferHandle()
	: bufferID(0) // Due to OpenGL syntax, we can't initial directly here, like we want.
{
	glGenBuffers(1, &bufferID);
}


BufferHandle::BufferHandle(BufferHandle&& other) noexcept
	: bufferID(std::move(other.bufferID))
{
	other.bufferID = 0;
}


BufferHandle& BufferHandle::operator=(BufferHandle&& other) noexcept {
	std::swap(bufferID, other.bufferID);
	return *this;
}


BufferHandle::~BufferHandle() {
	glDeleteBuffers(1, &bufferID);
}





//------------------------------------------------------------------------------

TextureHandle::TextureHandle()
	: textureID(0) // Due to OpenGL syntax, we can't initial directly here, like we want.
{
	glGenTextures(1, &textureID);
}


TextureHandle::TextureHandle(TextureHandle&& other) noexcept
	: textureID(std::move(other.textureID))
{
	other.textureID = 0;
}

TextureHandle& TextureHandle::operator=(TextureHandle&& other) noexcept {
	std::swap(textureID, other.textureID);
	return *this;
}


TextureHandle::~TextureHandle() {
	glDeleteTextures(1, &textureID);
}


TextureHandle::operator GLuint() const {
	return textureID;
}


GLuint TextureHandle::value() const {
	return textureID;
}
