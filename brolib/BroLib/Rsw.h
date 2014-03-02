#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <map>

class Rsm;

class Rsw
{
public:
	class Object
	{
	public:
		enum Type
		{
			Model,
			Light,
			Sound,
			Effect,
		} type;

		Object(Type type) : type(type) {};

		std::string name;

		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;

		virtual ~Object() {}
	};


	class Model : public Object
	{
	public:
		int animType;
		float animSpeed;
		int blockType;
		std::string fileName;
		std::string nodeName;

		Rsm* model;

		bool matrixCached;
		glm::mat4 matrixCache;

		Model() : Object(Object::Model)
		{
			model = NULL;
			matrixCached = false;
		}
		~Model();
	};

	class Light : public Object
	{
		std::string 	todo;
		glm::vec3		color;
		float			todo2;
		
		// custom properties!!!!!!!!!
		/*float		range;
		float		maxLightIncrement;
		bool		givesShadow;
		float		lightFalloff;*/
	};



	short version;
	std::string iniFile;
	std::string gndFile;
	std::string gatFile;

	struct  
	{
		float	height;
		int		type;
		float	amplitude;
		float	phase;
		float	surfaceCurve;
		int		animSpeed;
	} water;

	struct  
	{
		int			longitude;
		int			lattitude;
		glm::vec3	diffuse;
		glm::vec3	ambient;
		float		intensity;
	} light;

	int			unknown[4];
	std::vector<Object*> objects;


	std::map<std::string, Rsm*> rsmCache;

	Rsw(const std::string &fileName);
	Rsm* getRsw( const std::string &fileName );
};