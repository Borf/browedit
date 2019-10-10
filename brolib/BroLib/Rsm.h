#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <string>
#include <vector>
#include <string>
#include <map>
#include <functional>
#include "IRsm.h"

namespace blib
{
	class Texture;
	namespace util { class StreamInFile; class StreamOut; }
}

class RsmModelRenderInfo;
class RsmMeshRenderInfo;

class Rsm : public IRsm
{
public:
	class Mesh : public IRsm::Mesh<Rsm>
	{
	public:
		class Face : public IRsm::Face
		{
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

		glm::vec3						pos_;
		float							rotangle;
		glm::vec3						rotaxis;
		glm::vec3						scale;

		std::vector<int>				textures;
		std::vector<Frame*>				frames;

		glm::vec3 bbmin;
		glm::vec3 bbmax;
		glm::vec3 bbrange;

		void calcMatrix1();
		void calcMatrix2();
		bool matrixDirty = true;

		void setBoundingBox( glm::vec3& bbmin, glm::vec3& bbmax );
		void setBoundingBox2( glm::mat4& mat, glm::vec3& realbbmin, glm::vec3& realbbmax );
	};

public:
	Rsm(const std::string &fileName);
	~Rsm();
	void updateMatrices();
	void save(const std::string &fileName) const;


	int animLen;

	glm::vec3 realbbmin;
	glm::vec3 realbbmax;
	glm::vec3 realbbrange;

	glm::vec3 bbmin;
	glm::vec3 bbmax;
	glm::vec3 bbrange;

	float maxRange;

	char							unknown[16];
	char							alpha;
};

