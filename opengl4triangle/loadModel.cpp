#include "loadModel.h"


bool loadModel::loadMesh(const string& file)
{
	//First of all, clear any previously loaded meshes if indeed there is one.
	clear();
	bool ret = false; 
	Assimp::Importer import;
	const aiScene* pScene = import.ReadFile(file.c_str(), aiProcess_Triangulate 
						| aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
	if(pScene)
	{
		ret = initFromScene(pScene, file);
	} 
	else
	{
		printf("Error parsing '%s': '%s'\n", file.c_str(), import.GetErrorString());
	}
	return ret;
}

bool loadModel::initFromScene(const aiScene* pScene, const string& file)
{
	m_Entries.resize(pScene->mNumMeshes);
	m_Textures.resize(pScene->mNumMaterials);
	//initialise meshes of the scene one by one
	for (unsigned int i = 0; i< m_Entries.size();i++)
	{
		const aiMesh* paiMesh = pScene->mMeshes[i];
		initMesh(i, paiMesh);
	}
	return initMaterials(pScene, file);
}


void loadModel::initMesh(unsigned int ind, const aiMesh* paiMesh)
{
	m_Entries[ind].materialIndex = paiMesh->mMaterialIndex;
	vector<Vertex> verts;
	vector<unsigned int> inds;
	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
	for (unsigned int i = 0; i < paiMesh->mNumVertices;i++)
	{
		const aiVector3D* pPos = &(paiMesh->mVertices[i]);
		const aiVector3D* pNormal = &(paiMesh->mNormals[i]); // : &Zero3D;
		const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;
		Vertex v(Vec3f(pPos->x, pPos->y, pPos->z), 
						aiVector2D(pTexCoord->x, pTexCoord->y), 
						Vec3f(pNormal->x, pNormal->y, pNormal->z));
		verts.push_back(v);
	}
	for(unsigned int i = 0; i < paiMesh->mNumFaces; i++)
	{
		const aiFace& face = paiMesh->mFaces[i];
		assert(face.mNumIndices==3);
		inds.push_back(face.mIndices[0]);
		inds.push_back(face.mIndices[1]);
		inds.push_back(face.mIndices[2]);
	}
	m_Entries[ind].init(verts, inds);
}

bool loadModel::initMaterials(const aiScene* pScene, const string& file)
{
	string::size_type slashIndex = file.find_last_of("/");
	string dir;
	if(slashIndex == string::npos){dir=".";}
	else if (slashIndex==0){dir="/";}
	else{dir = file.substr(0, slashIndex);}
	bool ret = true;

	//initialise the materials
	for (unsigned int i = 0; i <pScene->mNumMaterials; i++)
	{
		const aiMaterial* pMaterial = pScene->mMaterials[i];
		//m_Textures[i] = NULL;
		if(pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		{
			aiString path;
			if(pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
			{
				string fullPath = dir + "/" + path.data;
				//m_Textures[i] = new glBindTexture(GL_TEXTURE_2D, fullPath.c_str());
			}
		}
	}
}








































//loadModel::loadModel()
//{
//	checker_texture = 0;
//	meshlist = NULL;
//}
//
//void loadModel::initChecker(void)
//{
//	int x, y, i;
//	x=y=i=0;
//	for(y;y<256;y++)
//	{
//		for(x;x<256;x++)
//		{
//			int k = ((x>>5)&1) ^ ((y>>5)&1);
//			checker_data[i++]= k? 255: 192;
//		}
//	}
//	glGenTextures(1, &checker_texture);
//	glBindTexture(GL_TEXTURE_2D, checker_texture);
//	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	glTexImage2D(GL_TEXTURE_2D, 0, 1, 256, 256, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, checker_data);
//}
//
//void loadModel::stringToLower(char *s)
//{
//	while (*s){*s = tolower(*s); s++;} //ensure lower case
//}
//
//unsigned int loadModel::loadTexture(char *fileName)
//{
//	unsigned int tex; 
//	unsigned char* img;
//	int w, h, n, intfmt = 0, fmt = 0;
//	img = stbi_load(fileName, &w, &h, &n, 0);
//	if(!img)
//	{
//		stringToLower(fileName);
//		img = stbi_load(fileName, &w, &h, &n, 0);
//		if(!img)
//		{
//			fprintf(stderr, "Could not load the texture image '%s'\n", fileName);
//			return 0;
//		}
//	}
//	if(n==1){intfmt = fmt = GL_LUMINANCE;}
//	if(n==2){intfmt = fmt = GL_LUMINANCE_ALPHA;}
//	if(n==3){intfmt = fmt = GL_RGB;}
//	if(n==4){intfmt = fmt = GL_RGBA;}
//	glGenTextures(1, &tex);
//	glBindTexture(GL_TEXTURE_2D, tex);
//	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	glTexImage2D(GL_TEXTURE_2D, 0, intfmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, img);
//	free(img);
//	return tex;
//}
