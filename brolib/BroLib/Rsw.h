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

		Object(Type type) : type(type), selected(false), matrixCached(false) {};

		std::string name;

		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;


		bool selected;
		bool matrixCached;
		glm::mat4 matrixCache;

		virtual ~Object() {}
	};


	class Model : public Object
	{
	public:
		int animType;
		float animSpeed;
		int blockType;
		std::string fileName;
		//std::string nodeName;

		Rsm* model;


		Model() : Object(Object::Model)
		{
			model = NULL;
		}
		~Model();
	};

	class Light : public Object
	{
	public:
		std::string 	todo;
		glm::vec3		color;
		float			todo2;
		
		Light() : Object(Object::Light)
		{
		}
		// custom properties!!!!!!!!!
		/*float		range;
		float		maxLightIncrement;
		bool		givesShadow;
		float		lightFalloff;*/
	};

	class Sound : public Object
	{
	public:
		Sound() : Object(Object::Sound)
		{
		}
		std::string fileName;
		float repeatDelay;
		float vol;
		long	width;
		long	height;
		float	range;
		float	cycle;
	};

	class Effect : public Object
	{
	public:
		Effect() : Object(Object::Effect)
		{
		}

		int	type;
		float loop;
		float param1;
		float param2;
		float param3;
		float param4;
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
	Rsm* getRsm( const std::string &fileName );
};