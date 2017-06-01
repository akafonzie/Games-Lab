#ifndef LOADMODEL_H
#define LOADMODEL_H


#include "include\GL\glew.h"
#include "include\GLFW\glfw3.h"
#include <vector> 
#include <string>
#include <map>

//#include "include\assimp\cimport.h"
#include "include\assimp\Importer.hpp"
#include "include\assimp\scene.h"
#include "include\assimp\postprocess.h"
#include "Vec3f.h"



using namespace std;
class loadModel{
public:
	loadModel();
	~loadModel();
	bool loadMesh(const string& file);
	void render();
private:
	bool initFromScene(const aiScene* pScene, const string& file);
	void initMesh(unsigned int ind, const aiMesh* paiMesh);
	bool initMaterials(const aiScene* pScene, const string& file);
	void clear();

	struct Vertex{
		Vec3f m_Pos;
		aiVector2D m_Tex;
		Vec3f m_Norm;
		Vertex();
		Vertex(const Vec3f& p, const aiVector2D& t, const Vec3f& n)
		{
			m_Pos	= p;
			m_Tex	= t; 
			m_Norm  = n;
		}
	};

#define INVALID_MATERIAL 0xFFFFFFFF
	struct meshEntry{
		meshEntry();
		~meshEntry();
		bool init(const vector<Vertex>& vertices, const vector<unsigned int>& indices);
		GLuint VB, IB;
		unsigned int numIndices, materialIndex;
	};


	vector<meshEntry> m_Entries;
	vector<aiMaterial> m_Textures;
};
#endif