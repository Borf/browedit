#pragma once

#include "../action.h"
#include <BroLib/Rsw.h>

class ObjectEditAction : public Action
{
	Rsw::Object* object;
public:
	ObjectEditAction(Rsw::Object* original);
	void setAfterAction();

	virtual void perform(Map* map, MapRenderer& mapRenderer);
	virtual void undo(Map* map, MapRenderer& mapRenderer);
};