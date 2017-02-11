#pragma once

#include "../action.h"
#include <BroLib/Rsw.h>

class ObjectEditAction : public Action
{
	Rsw::Object* object;
	int index;
	class Data
	{
	public:
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;
		std::string name;

		void fromObject(Rsw::Object* object);
		void toObject(Rsw::Object* object);
	};
	Data oldData;
	Data newData;
public:
	ObjectEditAction(Rsw::Object* original, int index);

	virtual void perform(Map* map, MapRenderer& mapRenderer);
	virtual void undo(Map* map, MapRenderer& mapRenderer);
	virtual bool isChanged();
};