#include "IRsm.h"


void IRsm::IMesh::fetchChildren(std::map<std::string, IMesh* > meshes)
{
	for (std::map<std::string, IMesh*, std::less<std::string> >::iterator it = meshes.begin(); it != meshes.end(); it++)
	{
		if (it->second->parentName == name && it->second != this)
		{
			it->second->parent = this;
			children.push_back(it->second);
		}
	}
	for (unsigned int i = 0; i < children.size(); i++)
		children[i]->fetchChildren(meshes);
}
