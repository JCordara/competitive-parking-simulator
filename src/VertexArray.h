#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H

#include <GL/glew.h>

#include <utility>
#include "GLHandles.h"

class VertexArray {

public:

	VertexArray();

	// Disallow copying
	VertexArray(const VertexArray&) = delete;
	VertexArray operator=(const VertexArray&) = delete;

	// Allow moving
	VertexArray(VertexArray&& other) noexcept;
	VertexArray& operator=(VertexArray&& other) noexcept;

	// Clean up after ourselves.
	~VertexArray();

	// Public interface
	void bind() const { glBindVertexArray(vaoID); }

	// Allow casting from this type into a GLuint
	// This allows usage in situations where a function expects a GLuint
	operator GLuint() const { return vaoID; }
	GLuint value() const { return vaoID; }

private:
	GLuint vaoID;
};

#endif