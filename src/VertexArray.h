#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H

#include <GL/glew.h>

#include <utility>
#include "GLHandles.h"

class VertexArray {

public:
	VertexArray();

	// Because we're using the VertexArrayHandle to do RAII for the shader for us
	// and our other types are trivial or provide their own RAII
	// we don't have to provide any specialized functions here. Rule of zero
	//
	// https://en.cppreference.com/w/cpp/language/rule_of_three
	// https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#Rc-zero

	// Public interface
	void bind() const { glBindVertexArray(arrayID); }
	void unbind() const { glBindVertexArray(arrayID); }

private:
	VertexArrayHandle arrayID;
};

#endif
