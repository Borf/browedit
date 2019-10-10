#pragma once

#include <glm/glm.hpp>
#include <functional>
#include <string>
#include <vector>
#include <map>

class RsmModelRenderInfo;
class RsmMeshRenderInfo;

class IRsm
{
public:
	class Face
	{
	public:
		short vertexIds[3];
		short texCoordIds[3];
		glm::vec3					normal;
		short texId;
		short twoSided;
		short smoothGroup;
		short padding;

	};


	class IMesh
	{
	public:
		std::string						name;
		std::string						parentName;

		glm::mat4						offset;
		glm::vec3						pos;

		std::vector<glm::vec3>			vertices;
		std::vector<glm::vec2>			texCoords;
		std::vector<Face*>				faces;
		std::vector<IMesh*> children;

		glm::mat4						matrix1;
		glm::mat4						matrix2;

		RsmMeshRenderInfo* renderer;
		IMesh* parent;

		void fetchChildren(std::map<std::string, IMesh* > meshes);

		void foreach(const std::function<void(IMesh*)>& callback)
		{
			callback(this);
			for (auto child : children)
				child->foreach(callback);
		}

		virtual void calcMatrix1() {}
		virtual void calcMatrix2() {}
	};

	template<class T>
	class Mesh : public IMesh
	{
	public:
		T* model;

		inline Mesh<T>* child(int i) { return (Mesh<T>*)children[i]; }
		
		void foreach(const std::function<void(Mesh<T>*)>& callback)
		{
			callback(this);
			for (auto child : children)
				child->foreach(callback);
		}

	};

	RsmModelRenderInfo* renderer;
	bool loaded;
	std::string fileName;
	std::vector<std::string> textures;
	short version;
	IMesh* rootMesh;

	enum eShadeType
	{
		SHADE_NO,
		SHADE_FLAT,
		SHADE_SMOOTH,
		SHADE_BLACK,
	}								shadeType;


	virtual void updateMatrices() {}
	virtual void save(const std::string& fileName) {}



};
