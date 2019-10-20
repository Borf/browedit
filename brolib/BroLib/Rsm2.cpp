#include "Rsm2.h"
#include <blib/util/FileSystem.h>
#include <blib/util/Log.h>

using blib::util::Log;

Rsm2::Rsm2(const std::string& fileName)
{
	this->fileName = fileName;
	renderer = NULL;
	loaded = false;
	rootMesh = nullptr;
	if (fileName.substr(fileName.size() - 5) != ".rsm2")
		return;

	blib::util::StreamInFile* rsmFile = new blib::util::StreamInFile(fileName);
	if (!rsmFile || !rsmFile->opened())
	{
		Log::out << "Unable to open " << fileName << Log::newline;
		delete rsmFile;
		return;
	}

	char header[4];
	rsmFile->read(header, 4);
	if (header[0] == 'G' && header[1] == 'R' && header[2] == 'G' && header[3] == 'M')
	{
		Log::out << "Unknown RSM header in file " << fileName << ", stopped loading" << Log::newline;
		delete rsmFile;
		return;
	}

	version = rsmFile->readShort();
	
	animationLength = rsmFile->readInt();
	shadeType = rsmFile->readInt();
	alpha = rsmFile->get();
	unknown = rsmFile->readFloat();

	if (version == 0x0202)
		load0202(rsmFile);
	if (version == 0x0203)
	{
		load0203(rsmFile);
		loaded = true;
	}

	if (rootMesh)
	{
		rootMesh->calcMatrix1();
		rootMesh->calcMatrix2();
	}



}


void Rsm2::load0202(blib::util::StreamInFile* rsmFile)
{

}


void Rsm2::load0203(blib::util::StreamInFile* rsmFile)
{
	int rootMeshCount = rsmFile->readInt();
	assert(rootMeshCount == 1);
	std::string rootMeshName = readString(rsmFile);

	int meshCount = rsmFile->readInt();
	std::map<std::string, IMesh* > meshes;

	for (int i = 0; i < meshCount; i++)
	{
		Mesh* mesh = new Mesh();
		mesh->name = readString(rsmFile);
		mesh->parentName = readString(rsmFile);
		int texCount = rsmFile->readInt();
		for (int i = 0; i < texCount; i++)
		{
			std::string texture = readString(rsmFile);
			auto f = std::find(textures.begin(), textures.end(), texture);
			if (f == textures.end())
			{
				mesh->textures.push_back(textures.size());
				textures.push_back(texture);
			}
			else
				mesh->textures.push_back(f - textures.begin());
		}

		mesh->offset = glm::mat4(1.0f);
		mesh->offset[0][0] = rsmFile->readFloat();//rotation
		mesh->offset[0][1] = rsmFile->readFloat();
		mesh->offset[0][2] = rsmFile->readFloat();

		mesh->offset[1][0] = rsmFile->readFloat();
		mesh->offset[1][1] = rsmFile->readFloat();
		mesh->offset[1][2] = rsmFile->readFloat();

		mesh->offset[2][0] = rsmFile->readFloat();
		mesh->offset[2][1] = rsmFile->readFloat();
		mesh->offset[2][2] = rsmFile->readFloat();

		mesh->pos = rsmFile->readVec3();

		int vertexCount = rsmFile->readInt();
		for (int i = 0; i < vertexCount; i++)
			mesh->vertices.push_back(rsmFile->readVec3());

		int texCoordCount = rsmFile->readInt();
		for (int i = 0; i < texCoordCount; i++)
		{
			glm::vec3 coord = rsmFile->readVec3();
			mesh->texCoords.push_back(glm::vec2(coord.y, coord.z));
		}

		int faceCount = rsmFile->readInt();
		for (int i = 0; i < faceCount; i++)
		{
			int len = rsmFile->readInt();
			Face* f = new Face();
			for (int ii = 0; ii < 3; ii++)
				f->vertexIds[ii] = rsmFile->readWord();
			for (int ii = 0; ii < 3; ii++)
				f->texCoordIds[ii] = rsmFile->readWord();
			f->texId = rsmFile->readWord();
			f->padding = rsmFile->readWord();
			f->twoSided = rsmFile->readWord();
			f->smoothGroup = rsmFile->readWord();

			rsmFile->readInt();
			//sometimes extra padding?
			for (int ii = 24; ii < len; ii++)
				rsmFile->get();
			mesh->faces.push_back(f);
		}

		int posKeyFramesCount = rsmFile->readInt();

		for (int i = 0; i < posKeyFramesCount; i++) {
			int frameId = rsmFile->readInt();
			glm::vec3 pos = rsmFile->readVec3();
			int unknown = rsmFile->readInt();
		}

		int rotFramesCount = rsmFile->readInt();

		for (int i = 0; i < rotFramesCount; i++)
		{
			int frame = rsmFile->readInt();
			float x = rsmFile->readFloat();
			float y = rsmFile->readFloat();
			float z = rsmFile->readFloat();
			float w = rsmFile->readFloat();
			glm::quat(w, x, y, z);
		}

		int unknownCount = rsmFile->readInt();

		for (int i = 0; i < unknownCount; i++)
		{
			char buf[20];
			rsmFile->read(buf, 20);
		}

		int unknownCount2 = rsmFile->readInt();

		for (int i = 0; i < unknownCount2; i++)
		{
			rsmFile->readInt();
			int unknownCount3 = rsmFile->readInt();

			for (int ii = 0; ii < unknownCount3; ii++)
			{
				int id = rsmFile->readInt();
				int unknownCount4 = rsmFile->readInt();
				for (int iii = 0; iii < unknownCount4; iii++) {
					int val1 = rsmFile->readInt();
					float val2 = rsmFile->readFloat();
				}
			}
		}

		if (meshes.find(mesh->name) != meshes.end())
			mesh->name += "(duplicate)";
		if (mesh->name == "")
			mesh->name = "empty";
		meshes[mesh->name] = mesh;
	}

	rootMesh = new Mesh();
	rootMesh->name = rootMeshName;
	rootMesh->fetchChildren(meshes);

}



void Rsm2::Mesh::calcMatrix1()
{
	matrix1 = glm::mat4();
	for (unsigned int i = 0; i < children.size(); i++)
		child(i)->calcMatrix1();
}
void Rsm2::Mesh::calcMatrix2()
{
	matrix2 = glm::mat4();
	matrix2 = glm::translate(matrix2, pos);
	matrix2 *= offset;

	for (unsigned int i = 0; i < children.size(); i++)
		child(i)->calcMatrix2();
}


std::string Rsm2::readString(blib::util::StreamInFile* file)
{
	int len = file->readInt();
	return file->readString(len, len);
}
