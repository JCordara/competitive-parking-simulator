#include "VertexArray.h"


VertexArray::VertexArray()
	: vaoID(0)
{
	glGenVertexArrays(1, &vaoID);
	bind();
}


VertexArray::VertexArray(VertexArray&& other) noexcept
	: vaoID(std::move(other.vaoID))
{
	other.vaoID = 0;
}


VertexArray& VertexArray::operator=(VertexArray&& other) noexcept {
	std::swap(vaoID, other.vaoID);
	return *this;
}


VertexArray::~VertexArray() {
	glDeleteVertexArrays(1, &vaoID);
}

