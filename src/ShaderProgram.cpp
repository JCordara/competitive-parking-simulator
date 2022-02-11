#include "ShaderProgram.h"


/* Constructor */

ShaderProgram::ShaderProgram() 
	: programID()
{}

ShaderProgram::ShaderProgram(const std::string& vertexPath, 
							 const std::string& fragmentPath)
	: ShaderProgram()
{
	addShader(GL_VERTEX_SHADER, vertexPath);
	addShader(GL_FRAGMENT_SHADER, fragmentPath);
}


/* Public interface */

void ShaderProgram::addShader(GLenum type, const std::string& vertexPath) {
	// Create shader in place and attach it by id
	glAttachShader(programID, shaders.emplace_back(vertexPath, type));
	link(); // Link each shader as its attached
}


bool ShaderProgram::recompile() {
	try {
		// Try to create a new program using the current shaders
		ShaderProgram newProgram;
		for (auto it = shaders.begin(); it != shaders.end(); it++) {
			newProgram.addShader(it->getType(), it->getPath());
		}

		// Attempt to replace this program with the newly created one
		*this = std::move(newProgram);
		return true;
	}
	catch (std::runtime_error) {
		Log::warn("SHADER_PROGRAM falling back to previous version of shaders");
		return false;
	}
}


/* Private methods */

bool ShaderProgram::link() const {

	GLint success;
	glLinkProgram(programID);
	glGetProgramiv(programID, GL_LINK_STATUS, &success);

	if (!success) {
		GLint logLength;
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logLength);
		std::vector<char> log(logLength);
		glGetProgramInfoLog(programID, logLength, NULL, log.data());

		// Get a list of shader paths with '\n's between
		std::string shaderPaths = "";
		for (auto it = shaders.begin(); it != shaders.end(); it++) {
			shaderPaths.append(it->getPath());
			shaderPaths.append("\n");
		}

		// Display which shaders weren't linking
		Log::error(
			"SHADER_PROGRAM linking {}\n{}", 
			shaderPaths.c_str(), 
			log.data()
		);

		// Delete The program and throw an error
		glDeleteProgram(programID);
		throw std::runtime_error("Shaders did not link.");
		return false;
	}
	else {
		// I hate this
		//Log::info("SHADER_PROGRAM successfully compiled and linked {} + {}"
		//		   , vertex.getPath(), fragment.getPath());
		return true;
	}
}
