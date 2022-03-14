#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <ShaderProgram.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <map>
#include <Texture.h>
#include <VertexArray.h>
#include <VertexBuffer.h>

using namespace std;
using vec3f = glm::vec3;
using vec2f = glm::vec2;
using filepath = std::string;
using filename = std::string;
using TextureMap = std::map<filename, unsigned int>;
using TextureMapPair = std::pair<filename, unsigned int>;

#define DEFAULT_UV vec2f(0.0,0.0);
#define NUM_MATERIAL_CONSTS 3 * 3 + 1

struct UniformMaterial {
	vec3f diff;
	vec3f spec;
	vec3f amb;
	float alpha;
};

class Mesh {
public:
	Mesh() = delete;
	Mesh(vector<vec3f> vertsPos, vector<vec3f> vertsNorms, vector<vec2f> vertsUVs, vector<GLuint> indicies, GLuint textureIndex, UniformMaterial material);
	//Reupload the data
	void uploadData();
	//Texture Binding will be handled by the model
	void draw(ShaderProgram& sp, GLint diffuseLocation, GLint specularLocation, GLint alphaLocation, GLint ambientLocation, unsigned int instances);
	vector<vec3f> vertsPos;
	vector<vec3f> vertsNorms;
	vector<vec2f> vertsUVs;
	vector<GLuint> indicies;
	GLuint textureIndex;
	UniformMaterial material;
private:
	std::shared_ptr<VertexArray> vao;
	std::shared_ptr<VertexBuffer> vertBuffer;
	std::shared_ptr<VertexBuffer> normalsBuffer;
	std::shared_ptr<VertexBuffer> texCoordBuffer;
	std::shared_ptr<IndexBuffer> indexBuffer;
};

class Model {
public:
	Model() = default;
	Model(filepath path, vec3f col) : modelColour(col) { loadModel(path); }
	void loadModel(filepath path);
	void draw(ShaderProgram& sp, GLenum textureLocation, GLint modelColourLocation, GLint diffuseLocation, GLint specularLocation, GLint alphaLocation, GLint ambientLocation, unsigned int instances);
	vector<Mesh> getMeshes() const;
private:
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	vector<Mesh> meshes;
	vector<std::shared_ptr<Texture>> textures;
	TextureMap textureMap;
	vec3f modelColour;
};


