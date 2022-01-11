#pragma once

#include "GLHandles.h"

#include <GL/glew.h>


class VertexArray {

public:
	VertexArray();

	// Public interface
	void bind() const { glBindVertexArray(arrayID); }

private:
	VertexArrayHandle arrayID;
};
