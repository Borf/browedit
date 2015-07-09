#include "Rsm.h"
#include "MapRenderer.h"

#include <blib/util/FileSystem.h>
#include <blib/linq.h>
#include <blib/util/Log.h>
#include <blib/util/Profiler.h>

using blib::util::Log;

#include <map>
#include <glm/gtc/matrix_transform.hpp>


Rsm::Rsm(std::string fileName)
{
	rootMesh = NULL;
	renderer = NULL;
	loaded = false;
	blib::util::StreamInFile* rsmFile = new blib::util::StreamInFile(fileName);
	if(!rsmFile || !rsmFile->opened())
	{
		Log::out<<"Unable to open "<<fileName<<Log::newline;
		return;
	}


	char header[4];
	rsmFile->read(header, 4);
	if(header[0] == 'G' && header[1] == 'R' && header[2] == 'G' && header[3] == 'M')
	{
		Log::out<<"Unknown RSM header in file "<<fileName<<", stopped loading"<<Log::newline;
		return;
	}

	version = rsmFile->readShort();
	animLen = rsmFile->readInt();
	shadeType = (eShadeType)rsmFile->readInt();
	if(version >= 0x0104)
		alpha = rsmFile->get();
	else
		alpha = 0;
	rsmFile->read(unknown, 16);


	int textureCount = rsmFile->readInt();

	for(int i = 0; i < textureCount; i++)
	{
		std::string textureFileName = rsmFile->readString(40);
		textures.push_back(textureFileName);
	}

	std::string mainNodeName = rsmFile->readString(40);
	int meshCount = rsmFile->readInt();
	std::map<std::string, Mesh* > meshes;
	for(int i = 0; i < meshCount; i++)
	{
		Mesh* mesh = new Mesh(this, rsmFile);
		meshes[mesh->name] = mesh;
	}

	if(meshes.find(mainNodeName) == meshes.end())
	{
		Log::out<<"cRsmModel "<<fileName<<": Could not locate root mesh"<<Log::newline;
		rootMesh = meshes.begin()->second;
	}
	else
		rootMesh = meshes[mainNodeName];

	rootMesh->parent = NULL;
	rootMesh->fetchChildren(meshes);



	bbmin = glm::vec3(999999, 999999, 999999);
	bbmax = glm::vec3(-999999, -999999, -9999999);
	rootMesh->setBoundingBox(bbmin, bbmax);
	bbrange = (bbmin + bbmax) / 2.0f;


	rootMesh->calcMatrix1();
	rootMesh->calcMatrix2();



	realbbmax = glm::vec3(-999999, -999999, -999999);
	realbbmin = glm::vec3(999999, 999999, 999999);
	glm::mat4 mat = glm::scale(glm::vec3(1,-1,1));
	rootMesh->setBoundingBox2(mat, realbbmin, realbbmax);
	realbbrange = (realbbmax + realbbmin) / 2.0f;
	maxRange = glm::max(glm::max(realbbmax.x, -realbbmin.x), glm::max(glm::max(realbbmax.y, -realbbmin.y), glm::max(realbbmax.z, -realbbmin.z)));


	loaded = true;
	delete rsmFile;
}


Rsm::~Rsm()
{
	if (renderer)
		delete renderer;
	if (rootMesh)
		delete rootMesh;
}


void Rsm::Mesh::calcMatrix1()
{
	matrix1 = glm::mat4();

	if(parent == NULL)
	{
		if(children.size() > 0)
			matrix1 = glm::translate(matrix1, glm::vec3(-model->bbrange.x, -model->bbmax.y, -model->bbrange.z));
		else
			matrix1 = glm::translate(matrix1, glm::vec3(0, -model->bbmax.y+model->bbrange.y, 0));
	}
	else
		matrix1 = glm::translate(matrix1, pos);

	if(frames.size() == 0)
	{
		if(fabs(rotangle) > 0.01)
			matrix1 = glm::rotate(matrix1, rotangle*180.0f/3.14159f, rotaxis);
	}
	else
	{
		if(true)
		{
			int tick = (int)(blib::util::Profiler::getAppTime() * 1000) % frames[frames.size() - 1]->time;

			int current = 0;
			for(unsigned int i = 0; i < frames.size(); i++)
			{
				if(frames[i]->time > tick)
				{
					current = i-1;
					break;
				}
			}
			if(current < 0)
				current = 0;

			int next = current+1;
			if(next >= (int)frames.size())
				next = 0;

			float interval = ((float) (tick-frames[current]->time)) / ((float) (frames[next]->time-frames[current]->time));
#if 1
			glm::quat quat = glm::mix(frames[current]->quaternion, frames[next]->quaternion, interval);
#else
			bEngine::math::cQuaternion quat(
				(1-interval)*animationFrames[current].quat.x + interval*animationFrames[next].quat.x,
				(1-interval)*animationFrames[current].quat.y + interval*animationFrames[next].quat.y,
				(1-interval)*animationFrames[current].quat.z + interval*animationFrames[next].quat.z,
				(1-interval)*animationFrames[current].quat.w + interval*animationFrames[next].quat.w);
#endif

			quat = glm::normalize(quat);

			matrix1 = matrix1 * glm::toMat4(quat);

		}
		else
		{
			matrix1 *= glm::toMat4(glm::normalize(frames[0]->quaternion));
 		}
 	}

	matrix1 = glm::scale(matrix1, scale);
//	if(nAnimationFrames == 0)
//		cache1 = true;

	for(unsigned int i = 0; i < children.size(); i++)
		children[i]->calcMatrix1();
}

void Rsm::Mesh::calcMatrix2()
{
	matrix2 = glm::mat4();

	if(parent == NULL && children.size() == 0)
		matrix2 = glm::translate(matrix2, -1.0f * model->bbrange);

	if(parent != NULL || children.size() != 0)
		matrix2 = glm::translate(matrix2, pos_);

	matrix2 *= offset;

	for(unsigned int i = 0; i < children.size(); i++)
		children[i]->calcMatrix2();
}




void Rsm::Mesh::setBoundingBox( glm::vec3& _bbmin, glm::vec3& _bbmax )
{
	int c;
	bbmin = glm::vec3(9999999, 9999999, 9999999);
	bbmax = glm::vec3(-9999999, -9999999, -9999999);

	if(parent != NULL)
		bbmin = bbmax = glm::vec3(0,0,0);

	glm::mat4 myMat = offset;
	for(unsigned int i = 0; i < faces.size(); i++)
	{
		for(int ii = 0; ii < 3; ii++)
		{
			glm::vec4 v = glm::vec4(vertices[faces[i]->vertices[ii]], 1);
			v = myMat * v;
			if(parent != NULL || children.size() != 0)
				v += glm::vec4(pos + pos_, 1);
			for(c = 0; c < 3; c++)
			{
				bbmin[c] = glm::min(bbmin[c], v[c]);
				bbmax[c] = glm::max(bbmax[c], v[c]);
			}
		}
	}
	bbrange = (bbmin + bbmax) / 2.0f;

	for(int c = 0; c < 3; c++)
		for(unsigned int i = 0; i < 3; i++)
		{
			_bbmax[c] = glm::max(_bbmax[c], bbmax[c]);
			_bbmin[c] = glm::min(_bbmin[c], bbmin[c]);
		}

		for(unsigned int i = 0; i < children.size(); i++)
			children[i]->setBoundingBox(_bbmin, _bbmax);
}

void Rsm::Mesh::setBoundingBox2( glm::mat4& mat, glm::vec3& bbmin_, glm::vec3& bbmax_)
{
	glm::mat4 mat1 = mat * matrix1;
	glm::mat4 mat2 = mat * matrix1 * matrix2;
	for(unsigned int i = 0; i < faces.size(); i++)
	{
		for(int ii = 0; ii < 3; ii++)
		{
			glm::vec4 v = mat2 * glm::vec4(vertices[faces[i]->vertices[ii]],1);
			bbmin_.x = glm::min(bbmin_.x, v.x);
			bbmin_.y = glm::min(bbmin_.y, v.y);
			bbmin_.z = glm::min(bbmin_.z, v.z);

			bbmax_.x = glm::max(bbmax_.x, v.x);
			bbmax_.y = glm::max(bbmax_.y, v.y);
			bbmax_.z = glm::max(bbmax_.z, v.z);
		}
	}

	for(unsigned int i = 0; i < children.size(); i++)
		children[i]->setBoundingBox2(mat1, bbmin_, bbmax_);	
}



Rsm::Mesh::Mesh(Rsm* model, blib::util::StreamInFile* rsmFile)
{
	renderer = NULL;
	this->model = model;
	name = rsmFile->readString(40);
	parentName = rsmFile->readString(40);

	int textureCount = rsmFile->readInt();
//TODO!
	for (int i = 0; i < textureCount; i++)
		textures.push_back(rsmFile->readInt());

#if 1
	offset[0][0] = rsmFile->readFloat();//rotation
	offset[0][1] = rsmFile->readFloat();
	offset[0][2] = rsmFile->readFloat();

	offset[1][0] = rsmFile->readFloat();
	offset[1][1] = rsmFile->readFloat();
	offset[1][2] = rsmFile->readFloat();

	offset[2][0] = rsmFile->readFloat();
	offset[2][1] = rsmFile->readFloat();
	offset[2][2] = rsmFile->readFloat();
#else
	offset[0][0] = rsmFile->readFloat();//rotation
	offset[1][0] = rsmFile->readFloat();
	offset[2][0] = rsmFile->readFloat();

	offset[0][1] = rsmFile->readFloat();
	offset[1][1] = rsmFile->readFloat();
	offset[2][1] = rsmFile->readFloat();

	offset[0][2] = rsmFile->readFloat();
	offset[1][2] = rsmFile->readFloat();
	offset[2][2] = rsmFile->readFloat();
#endif
	pos_ = rsmFile->readVec3();
	pos = rsmFile->readVec3();
	rotangle = rsmFile->readFloat();
	rotaxis = rsmFile->readVec3();
	scale = rsmFile->readVec3();

	int vertexCount = rsmFile->readInt();
	vertices.resize(vertexCount);
	for(int i = 0; i < vertexCount; i++)
		vertices[i] = rsmFile->readVec3();

	int texCoordCount = rsmFile->readInt();
	texCoords.resize(texCoordCount);
	for(int i = 0; i < texCoordCount; i++)
	{
		if(model->version >= 0x0102)
			rsmFile->readFloat();
		texCoords[i] = rsmFile->readVec2();
		//texCoords[i].y = 1-texCoords[i].y;
	}

	int faceCount = rsmFile->readInt();
	faces.resize(faceCount);
	for(int i = 0; i < faceCount; i++)
	{
		Face* f = new Face();
		f->vertices[0] = rsmFile->readWord();
		f->vertices[1] = rsmFile->readWord();
		f->vertices[2] = rsmFile->readWord();
		f->texvertices[0] = rsmFile->readWord();
		f->texvertices[1] = rsmFile->readWord();
		f->texvertices[2] = rsmFile->readWord();

		f->texIndex = rsmFile->readWord();
		rsmFile->readWord();//TODO?
		f->twoSide = rsmFile->readInt();
		f->smoothGroup = rsmFile->readInt();

		faces[i] = f;
		
		f->normal = glm::normalize(glm::cross(vertices[f->vertices[1]] - vertices[f->vertices[0]], vertices[f->vertices[2]] - vertices[f->vertices[0]]));
	}

	int frameCount = rsmFile->readInt();
	frames.resize(frameCount);
	for(int i = 0; i < frameCount; i++)
	{
		frames[i] = new Frame();
		frames[i]->time = rsmFile->readInt();

		float x,y,z,w;

		x = rsmFile->readFloat();
		y = rsmFile->readFloat();
		z = rsmFile->readFloat();
		w = rsmFile->readFloat();

		frames[i]->quaternion = glm::quat( w, x, y, z );
	}
//	calcVbos();
}


Rsm::Mesh::~Mesh()
{
	if (renderer)
		delete renderer;
	blib::linq::deleteall(faces);
	blib::linq::deleteall(frames);
	blib::linq::deleteall(children);
}




/*
void Rsm::Mesh::calcVbos()
{
	std::map<int, std::vector<VertexPositionNormalTexture> > verts;

	for(unsigned int i = 0; i < faces.size(); i++)
	{
		Face* face = faces[i];
		for(int c = 0; c < 3; c++)
		{
			verts[face->texIndex].push_back(VertexPositionNormalTexture(vertices[face->vertices[c]], face->normal, texCoords[face->texvertices[c]]));
		}
	}
	for(std::map<int, std::vector<VertexPositionNormalTexture> >::iterator it = verts.begin(); it != verts.end(); it++)
	{
		vbos[it->first] = new VBO<VertexPositionNormalTexture>();
		vbos[it->first]->setData(it->second.size(), &it->second[0], GL_STATIC_DRAW);
		vaos[it->first] = new VAO<VertexPositionNormalTexture>(vbos[it->first]);
	}
}




void Rsm::Mesh::draw(WorldShader* shader, glm::mat4 modelMatrix)
{
	modelMatrix *= matrix1;
	shader->setModelMatrix(modelMatrix * matrix2);
	for (std::map<int, VAO<VertexPositionNormalTexture>* >::iterator it = vaos.begin(); it != vaos.end(); it++)
	{
		glBindTexture(GL_TEXTURE_2D, textures[it->first]->texid);
		it->second->bind();
		glDrawArrays(GL_TRIANGLES, 0, vbos[it->first]->getLength());
		//it->second->unBind();
	}
	for(unsigned int i = 0; i < children.size(); i++)
	{
		children[i]->draw(shader, modelMatrix);
	}
}
*/
void Rsm::Mesh::fetchChildren( std::map<std::string, Mesh* > meshes )
{
	for(std::map<std::string, Mesh*, std::less<std::string> >::iterator it = meshes.begin(); it != meshes.end(); it++)
	{
		if(it->second->parentName == name && it->second != this)
		{
			it->second->parent = this;
			children.push_back(it->second);
		}
	}
	for(unsigned int i = 0; i < children.size(); i++)
		children[i]->fetchChildren(meshes);
}


/*
void Rsm::draw(WorldShader* shader, glm::mat4 modelMatrix)
{
	rootMesh->draw(shader, modelMatrix);
}
*/

