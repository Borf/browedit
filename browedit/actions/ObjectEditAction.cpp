#include "ObjectEditAction.h"
#include <BroLib/Rsw.h>
#include <BroLib/Map.h>


ObjectEditAction::ObjectEditAction(Rsw::Object* original, int index)
{
	this->object = original;
	this->index = index;
	oldData.fromObject(original);
}


void ObjectEditAction::perform(Map* map, MapRenderer& mapRenderer)
{
	if (object)
	{
		newData.fromObject(object);
		object = NULL;
	}
	else
	{
		newData.toObject(map->getRsw()->objects[index]);
	}
}

void ObjectEditAction::undo(Map* map, MapRenderer& mapRenderer)
{
	oldData.toObject(map->getRsw()->objects[index]);
}

bool ObjectEditAction::isChanged()
{
	if (oldData.position != object->position ||
		oldData.rotation != object->rotation ||
		oldData.scale != object->scale)
		return true;
	return false;
}


void ObjectEditAction::Data::fromObject(Rsw::Object* object)
{
	position = object->position;
	rotation = object->rotation;
	scale = object->scale;
	name = object->name;
}

void ObjectEditAction::Data::toObject(Rsw::Object* object)
{
	object->matrixCached = false;


	object->position = position;
	object->rotation = rotation;
	object->scale = scale;
	object->name = name;

}
