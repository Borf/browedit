#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <blib/util/Tree.h>
#include <blib/math/AABB.h>
class Gnd;
class Rsm;

class Rsw
{
public:
	class Object
	{
	public:
		enum class Type
		{
			Model,
			Light,
			Sound,
			Effect,
		} type;

		Object(Type type) : type(type), selected(false), matrixCached(false), aabb(glm::vec3(0,0,0), glm::vec3(0,0,0)) {};

		std::string name;

		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;


		bool selected;
		bool matrixCached;
		glm::mat4 matrixCache;

		blib::math::AABB aabb;

		virtual ~Object() {}
		virtual bool collides(const blib::math::Ray &ray) { return false; };
		virtual std::vector<glm::vec3> collisions(const blib::math::Ray &ray) { return std::vector<glm::vec3>();  };
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
		bool collides(const blib::math::Ray &ray);
		std::vector<glm::vec3> collisions(const blib::math::Ray &ray);
		void foreachface(std::function<void(const std::vector<glm::vec3>&)> callback);


		Model() : Object(Object::Type::Model)
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
		
		Light() : Object(Object::Type::Light)
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
		Sound() : Object(Object::Type::Sound)
		{
		}
		std::string fileName;
		//float repeatDelay;
		float vol;
		long	width;
		long	height;
		float	range;
		float	cycle;
	};

	class Effect : public Object
	{
	public:
		Effect() : Object(Object::Type::Effect)
		{
		}

		int	id;
		float loop;
		float param1;
		float param2;
		float param3;
		float param4;
	};


	class QuadTreeNode : public blib::util::Tree<4, QuadTreeNode>
	{
	public:
        QuadTreeNode(std::vector<glm::vec3>::const_iterator &it, int level = 0);
		~QuadTreeNode();
		blib::math::AABB bbox;
		glm::vec3 range[2];
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
		int			latitude;
		glm::vec3	diffuse;
		glm::vec3	ambient;
		float		intensity;
	} light;

	int			unknown[4];
	std::vector<Object*> objects;
	std::vector<glm::vec3> quadtreeFloats;
	QuadTreeNode* quadtree;

	std::map<std::string, Rsm*> rsmCache;

	Rsw(const std::string &fileName, bool loadModels = true);
	Rsw(int width, int height);
	~Rsw();

	Rsm* getRsm( const std::string &fileName );
	void save(const std::string &fileName);
	void recalculateQuadTree(Gnd* gnd);
};
