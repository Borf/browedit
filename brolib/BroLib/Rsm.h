#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <string>
#include <vector>
#include <string>
#include <map>
#include <functional>

namespace blib
{
	class Texture;
	namespace util { class StreamInFile; class StreamOut; }
}

class RsmModelRenderInfo;
class RsmMeshRenderInfo;

class Rsm
{
public:
	class Mesh
	{
	public:
		class Face
		{
		public:
			int							vertices[3];
			int							texvertices[3];
			glm::vec3					normal;
			int							texIndex;
			int							twoSide;
			int							smoothGroup;
		};

		class Frame
		{
		public:
			int							time;
			glm::quat					quaternion;
		};


		Mesh(Rsm* model, blib::util::StreamInFile* rsmFile);
		Mesh(Rsm* model);
		~Mesh();

		void save(blib::util::StreamOut* pFile);
		void foreach(const std::function<void(Mesh*)> &callback);

		std::string						name;
		std::string						parentName;

		glm::mat4						offset;
		glm::vec3						pos_;
		glm::vec3						pos;
		float							rotangle;
		glm::vec3						rotaxis;
		glm::vec3						scale;

		std::vector<int>				textures;
		std::vector<glm::vec3>			vertices;
		std::vector<glm::vec2>			texCoords;
		std::vector<Face*>				faces;
		std::vector<Frame*>				frames;


		RsmMeshRenderInfo*				renderer;
		Mesh* parent;
		Rsm* model;
		std::vector<Mesh*> children;
		glm::vec3 bbmin;
		glm::vec3 bbmax;
		glm::vec3 bbrange;

		glm::mat4 matrix1;
		glm::mat4 matrix2;
		void calcMatrix1();
		void calcMatrix2();
		bool matrixDirty = true;

		void setBoundingBox( glm::vec3& bbmin, glm::vec3& bbmax );
		void setBoundingBox2( glm::mat4& mat, glm::vec3& realbbmin, glm::vec3& realbbmax );
		void fetchChildren( std::map<std::string, Mesh* > meshes );
	};

public:
	Rsm(const std::string &fileName);
	~Rsm();
	void updateMatrices();
	void save(const std::string &fileName) const;

	bool loaded;

	short version;
	int animLen;
	std::string fileName;
	std::vector<std::string> textures;

	glm::vec3 realbbmin;
	glm::vec3 realbbmax;
	glm::vec3 realbbrange;

	glm::vec3 bbmin;
	glm::vec3 bbmax;
	glm::vec3 bbrange;

	float maxRange;

	Mesh* rootMesh;
	RsmModelRenderInfo*					renderer;

	enum eShadeType
	{
		SHADE_NO,
		SHADE_FLAT,
		SHADE_SMOOTH,
		SHADE_BLACK,
	}								shadeType;

	char							unknown[16];
	char							alpha;
};

