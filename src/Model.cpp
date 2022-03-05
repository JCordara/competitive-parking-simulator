#pragma once
#include "Model.h"

Mesh::Mesh(vector<vec3f> vertsPos, vector<vec3f> vertsNorms, vector<vec2f> vertsUVs, vector<GLuint> indicies, GLuint textureIndex, UniformMaterial material) :
	vertsPos(vertsPos),
	vertsNorms(vertsNorms),
	vertsUVs(vertsUVs),
	indicies(indicies),
	textureIndex(textureIndex),
	material(material){
	vao = std::make_shared<VertexArray>();
	vertBuffer = std::make_shared<VertexBuffer>(0, 3, GL_FLOAT);
	normalsBuffer = std::make_shared<VertexBuffer>(1, 3, GL_FLOAT);
	texCoordBuffer = std::make_shared<VertexBuffer>(2, 2, GL_FLOAT);
	indexBuffer = std::make_shared<IndexBuffer>(3, 1, GL_UNSIGNED_INT);

}


void Mesh::uploadData() {
	//Prep Mesh Data
	vao->bind();
	vertBuffer->uploadData(sizeof(vec3f) * vertsPos.size(), vertsPos.data(), GL_STATIC_DRAW);
	normalsBuffer->uploadData(sizeof(vec3f) * vertsNorms.size(), vertsNorms.data(), GL_STATIC_DRAW);
	texCoordBuffer->uploadData(sizeof(vec2f) * vertsUVs.size(), vertsUVs.data(), GL_STATIC_DRAW);
	indexBuffer->uploadData(sizeof(GLuint) * indicies.size(), indicies.data(), GL_STATIC_DRAW);
	vao->unbind();
}

//Texture Binding will be handled by the model
void Mesh::draw(ShaderProgram& sp, GLint diffuseLocation, GLint specularLocation, GLint alphaLocation, GLint ambientLocation, unsigned int instances = 1) {
	//Material Constants
	vao->bind();
	if (diffuseLocation > -1) glUniform3fv(diffuseLocation, 1, &material.diff[0]);
	if (specularLocation > -1) glUniform3fv(specularLocation, 1, &material.spec[0]);
	if (ambientLocation > -1) glUniform3fv(ambientLocation, 1, &material.amb[0]);
	if (alphaLocation > -1) glUniform1f(alphaLocation, material.alpha);
	//Render Mesh
	if (instances == 1) glDrawElements(GL_TRIANGLES, indicies.size(), GL_UNSIGNED_INT, 0);
	else if (instances > 1) glDrawElementsInstanced(GL_TRIANGLES, indicies.size(), GL_UNSIGNED_INT, 0, instances);
	vao->unbind();
}

//----------------------------------------------------------------------------------------------------------------------

void Model::loadModel(filepath path) {
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
		return;
	}
	processNode(scene->mRootNode, scene);
}
void Model::draw(ShaderProgram& sp, GLenum textureLocation, GLint modelColourLocation, GLint diffuseLocation, GLint specularLocation, GLint alphaLocation, GLint ambientLocation, unsigned int instances = 1) {
	if(modelColourLocation > -1) glUniform3fv(modelColourLocation, 1, &modelColour[0]);
	for (unsigned int i = 0, textureid; i < meshes.size(); i++) {
		textureid = meshes[i].textureIndex;
		if (textureid >= 0) {
			glActiveTexture(textureLocation);
			textures[textureid]->bind();
			meshes[i].draw(sp, diffuseLocation, specularLocation, alphaLocation, ambientLocation, instances);
			textures[textureid]->unbind();
		}
	}
}
	void Model::processNode(aiNode* node, const aiScene* scene) {
		meshes.reserve(node->mNumMeshes);
		for (unsigned int i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			if (mesh->HasFaces() && mesh->HasNormals() && mesh->HasPositions() && mesh->HasTextureCoords(0)) {
				meshes.push_back(processMesh(mesh, scene));
				meshes[meshes.size() - 1].uploadData();
			}
			else
				std::cerr << "Mesh not supported in scene: " << "TO-DO: Mesh loader Error printer" << std::endl;
		}
		for (unsigned int i = 0; i < node->mNumChildren; i++)
			processNode(node->mChildren[i], scene);
	}
	Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
		vector<vec3f> vertsPos;
		vector<vec3f> vertsNorms;
		vector<vec2f> vertsUVs;
		vector<GLuint> indicies;
		GLuint textureIndex;
		UniformMaterial material;
		aiVector3D vec;
		vertsPos.reserve(mesh->mNumVertices);
		vertsNorms.reserve(mesh->mNumVertices);
		vertsUVs.reserve(mesh->mNumVertices);
		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			vec = mesh->mVertices[i];
			vertsPos.push_back(glm::vec3(vec.x, vec.y, vec.z));
			vec = mesh->mNormals[i];
			vertsNorms.push_back(glm::vec3(vec.x, vec.y, vec.z));
			if (mesh->mTextureCoords[0] != NULL) {
				vec = mesh->mTextureCoords[0][i];
				vertsUVs.push_back(glm::vec2(vec.x, vec.y));
			}
			else vertsUVs[i] = DEFAULT_UV;
		}
		indicies.reserve(mesh->mNumFaces * 3);
		for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
			indicies.push_back(mesh->mFaces[i].mIndices[0]);
			indicies.push_back(mesh->mFaces[i].mIndices[1]);
			indicies.push_back(mesh->mFaces[i].mIndices[2]);
		}
		aiMaterial* _material = scene->mMaterials[mesh->mMaterialIndex];
		aiColor4D dif;
		aiColor4D spec;
		aiColor4D amb;
		float alpha;
		aiGetMaterialColor(_material, AI_MATKEY_COLOR_DIFFUSE, &dif);
		aiGetMaterialColor(_material, AI_MATKEY_COLOR_SPECULAR, &spec);
		aiGetMaterialColor(_material, AI_MATKEY_COLOR_AMBIENT, &amb);
		aiGetMaterialFloat(_material, AI_MATKEY_SHININESS, &alpha);
		material.diff = vec3f(dif.r, dif.b, dif.r);
		material.spec = vec3f(spec.r, spec.b, spec.r);
		material.amb = vec3f(amb.r, amb.b, amb.r);
		material.alpha = alpha;
		//Texture-------------------------
		if (_material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			aiString _textureName;
			_material->GetTexture(aiTextureType_DIFFUSE, 0, &_textureName);
			filename fn(_textureName.C_Str());
			auto it = textureMap.find(fn);
			if (it != textureMap.end())//Already exists
				textureIndex = it->second;
			else {
				try {
					textureIndex = textures.size();
					textureMap.insert(TextureMapPair(fn, textureIndex));
					auto tex = std::make_shared<Texture>();
					textures.push_back(tex);
					textures[textureIndex]->load("textures/" + fn, GL_NEAREST);
				}
				catch (std::runtime_error e) {
					std::cerr << e.what() << std::endl;
					textureIndex = -1;
				}
			}
		}
		else {
			std::cerr << "Material has no texture" << std::endl;
			textureIndex = -1;
		}
		//end Texture----------------------
		return Mesh(vertsPos, vertsNorms, vertsUVs, indicies, textureIndex, material);
	}


	vector<Mesh> Model::getMeshes() {
		return meshes;
	}
