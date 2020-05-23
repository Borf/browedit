#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <blib/util/Tree.h>
#include <blib/math/AABB.h>
#include <blib/util/Watchable.h>
class Gnd;
class IRsm;
class Map;

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

		Object(Type type) : type(type), selected(false), matrixCached(false) {};

		std::string name;

		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;


		bool selected;
		bool matrixCached;
		glm::mat4 matrixCache;

		virtual ~Object() {}
		virtual bool collides(const blib::math::Ray &ray, Map* map) { return false; };
		virtual std::vector<glm::vec3> collisions(const blib::math::Ray &ray, Map* map) { return std::vector<glm::vec3>();  };
	};


	class Model : public Object
	{
	public: 
		blib::math::AABB aabb;


		int animType;
		float animSpeed;
		int blockType;
		std::string fileName;
		//std::string nodeName;

		IRsm* model;
		bool collides(const blib::math::Ray &ray, Map* map);
		std::vector<glm::vec3> collisions(const blib::math::Ray &ray, Map* map) override;
		void foreachface(std::function<void(const std::vector<glm::vec3>&)> callback);
		bool collidesTexture(const blib::math::Ray &ray);

		void getWorldVerts(std::vector<int> &indices, std::vector<glm::vec3> &vertices);

		Model() : Object(Object::Type::Model), aabb(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0))
		{
			model = NULL;
		}
		~Model();


	};

	class SpriteObject : public Object
	{
	public:
		SpriteObject(Type type) : Object(type) {}

		virtual bool collides(const blib::math::Ray& ray, Map* map) override;
		virtual std::vector<glm::vec3> collisions(const blib::math::Ray& ray, Map* map) override;
	};

	class Light : public SpriteObject
	{
	public:
		float todo[10];
		glm::vec3		color;
		float			todo2;
		
		Light() : SpriteObject(Object::Type::Light)
		{
		}


		// custom properties!!!!!!!!!
		enum class Type
		{
			Point,
			Spot
		} type;
		float		range;
		//float		maxLightIncrement;
		bool		givesShadow;
		//float		lightFalloff;

		float cutOff;
		float intensity;

		float realRange();
	};

	class Sound : public SpriteObject
	{
	public:
		Sound() : SpriteObject(Object::Type::Sound)
		{
		}
		std::string fileName;
		//float repeatDelay;
		float vol;
		long	width;
		long	height;
		float	range;
		float	cycle;


		char unknown6[8];
		float unknown7;
		float unknown8;
	};

	class Effect : public SpriteObject
	{
	public:
		Effect() : SpriteObject(Object::Type::Effect)
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
		blib::util::Watchable<int>		type;
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
		float		lightmapAmbient = 0.5f; //EXTRA
		float		lightmapIntensity = 0.5f; // EXTRA
	} light;

	int			unknown[4];
	std::vector<Object*> objects;
	std::vector<glm::vec3> quadtreeFloats;
	QuadTreeNode* quadtree;

	std::map<std::string, IRsm*> rsmCache;

	Rsw(const std::string &fileName, bool loadModels = true);
	Rsw(int width, int height);
	~Rsw();

	IRsm* getRsm( const std::string &fileName );
	void save(const std::string &fileName);
	void recalculateQuadTree(Gnd* gnd);
};
