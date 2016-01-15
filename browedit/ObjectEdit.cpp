#include "BrowEdit.h"
#include "Camera.h"

#include <blib/wm/WM.h>
#include <blib/util/Log.h>
#include <blib/Math.h>

#include <BroLib/Map.h>
#include <BroLib/Gnd.h>

#include "actions/SelectObjectAction.h"
#include "actions/ObjectEditAction.h"
#include "actions/GroupAction.h"

#include "windows/ModelPropertiesWindow.h"
#include "windows/ObjectWindow.h"

using blib::util::Log;

void BrowEdit::objectEditUpdate()
{
	if (!wm->inWindow(mouseState.position))
	{
		if (newModel)
		{
			newModel->position = glm::vec3(mapRenderer.mouse3d.x - map->getGnd()->width * 5, -mapRenderer.mouse3d.y, -mapRenderer.mouse3d.z + (10 + 5 * map->getGnd()->height));;
			newModel->matrixCached = false;
			if (mouseState.leftButton && !lastMouseState.leftButton)
				newModel = NULL;
		}

		else if (mouseState.leftButton && !lastMouseState.leftButton)
		{//down
			startMouseState = mouseState;
			mouse3dstart = mapRenderer.mouse3d;

			selectObjectAction = new SelectObjectAction(map->getRsw());
			glm::vec3 center;
			int selectCount = 0;
			for (size_t i = 0; i < map->getRsw()->objects.size(); i++)
			{
				if (!map->getRsw()->objects[i]->selected)
					continue;
				selectCount++;
				center += glm::vec3(5 * map->getGnd()->width + map->getRsw()->objects[i]->position.x, -map->getRsw()->objects[i]->position.y, 10 + 5 * map->getGnd()->height - map->getRsw()->objects[i]->position.z);
			}

			if (selectCount > 0)
			{
				center /= selectCount;
				objectTranslateDirection = TranslatorTool::Axis::NONE;
				objectRotateDirection = RotatorTool::Axis::NONE;
				objectScaleDirection = ScaleTool::Axis::NONE;

				if (objectEditModeTool == ObjectEditModeTool::Translate)
					objectTranslateDirection = translatorTool.selectedAxis(mapRenderer.mouseRay, center);
				if (objectEditModeTool == ObjectEditModeTool::Rotate)
					objectRotateDirection = rotatorTool.selectedAxis(mapRenderer.mouseRay, center);
				if (objectEditModeTool == ObjectEditModeTool::Scale)
					objectScaleDirection = scaleTool.selectedAxis(mapRenderer.mouseRay, center);

				if (objectEditModeTool == ObjectEditModeTool::Translate && objectTranslateDirection == TranslatorTool::Axis::NONE)
				{//check if clicked on a selected model
					glm::vec3 cameraPos(camera->getMatrix() * glm::vec4(0, 0, 0, 1));
					cameraPos = glm::vec3(1, 1, -1) * cameraPos;
					Rsw::Object* closestObject = NULL;
					float closestDist = 9999999999.0f;
					for (size_t i = 0; i < map->getRsw()->objects.size(); i++)
					{
						Rsw::Object* o = map->getRsw()->objects[i];
						glm::vec2 pos = glm::vec2(map->getGnd()->width * 5 + o->position.x, 10 + 5 * map->getGnd()->height - o->position.z);
						float dist = glm::length(pos - glm::vec2(mapRenderer.mouse3d.x, mapRenderer.mouse3d.z));
						std::vector<glm::vec3> collisions = o->collisions(mapRenderer.mouseRay);
						if (!collisions.empty())
							objectTranslateDirection = TranslatorTool::Axis::XYZ;
					}
				}

				for (size_t i = 0; i < map->getRsw()->objects.size(); i++)
					if (map->getRsw()->objects[i]->selected)
						objectEditActions.push_back(new ObjectEditAction(map->getRsw()->objects[i], i));
			}




		}
		else if (!mouseState.leftButton && lastMouseState.leftButton)
		{//left up
			if (objectTranslateDirection != TranslatorTool::Axis::NONE || objectRotateDirection != RotatorTool::Axis::NONE || objectScaleDirection != ScaleTool::Axis::NONE)
			{
				GroupAction* action = new GroupAction();
				for (ObjectEditAction* a : objectEditActions)
					action->add(a);
				perform(action);
				objectEditActions.clear();
			}
			else if (abs(startMouseState.position.x - lastMouseState.position.x) < 2 && abs(startMouseState.position.y - lastMouseState.position.y) < 2)
			{ //click
				if (!wm->inWindow(mouseState.position))
				{
					glm::vec3 cameraPos(camera->getMatrix() * glm::vec4(0, 0, 0, 1));
					cameraPos = glm::vec3(1,1,-1) * cameraPos;
					Rsw::Object* closestObject = NULL;
					float closestDist = 9999999999.0f;
					for (size_t i = 0; i < map->getRsw()->objects.size(); i++)
					{
						Rsw::Object* o = map->getRsw()->objects[i];
						glm::vec2 pos = glm::vec2(map->getGnd()->width * 5 + o->position.x, 10 + 5 * map->getGnd()->height - o->position.z);
						float dist = glm::length(pos - glm::vec2(mapRenderer.mouse3d.x, mapRenderer.mouse3d.z));
						std::vector<glm::vec3> collisions = o->collisions(mapRenderer.mouseRay);
						o->selected = false;
						if (collisions.size() > 0)
						{
							for(glm::vec3& c : collisions)
							{
								if (glm::distance(c, cameraPos) < closestDist)
								{
									closestDist = glm::distance(c, cameraPos);
									closestObject = o;
								}
							}
						}
					}

					if (closestObject)
					{
						closestObject->selected = closestObject->collides(mapRenderer.mouseRay);
						if (closestObject->selected && mouseState.clickcount == 2)
						{
							if (closestObject->type == Rsw::Object::Type::Model)
								new ModelPropertiesWindow((Rsw::Model*)closestObject, resourceManager, this);
						}
					}

				}
			}
			else
			{
				for (size_t i = 0; i < map->getRsw()->objects.size(); i++)
				{
					Rsw::Object* o = map->getRsw()->objects[i];
					glm::vec2 tl = glm::vec2(glm::min(mouse3dstart.x, mapRenderer.mouse3d.x), glm::min(mouse3dstart.z, mapRenderer.mouse3d.z));
					glm::vec2 br = glm::vec2(glm::max(mouse3dstart.x, mapRenderer.mouse3d.x), glm::max(mouse3dstart.z, mapRenderer.mouse3d.z));
					glm::vec2 pos = glm::vec2(map->getGnd()->width * 5 + o->position.x, 10 + 5 * map->getGnd()->height - o->position.z);

					o->selected = pos.x > tl.x && pos.x < br.x && pos.y > tl.y && pos.y < br.y;
				}
			}
			if (selectObjectAction)
			{
				selectObjectAction->finish(map->getRsw());
				if (selectObjectAction->hasDifference())
					perform(selectObjectAction);
				else
				{
					delete selectObjectAction;
					selectObjectAction = NULL;
				}
			}
		}
		else if (mouseState.leftButton && lastMouseState.leftButton)
		{ // dragging				
			glm::vec3 center(0, 0, 0);
			int count = 0;
			for (size_t i = 0; i < map->getRsw()->objects.size(); i++)
				if (map->getRsw()->objects[i]->selected)
				{
					center += map->getRsw()->objects[i]->position;
					count++;
				}
			if (count > 0)
			{
				center /= count;


				for (size_t i = 0; i < map->getRsw()->objects.size(); i++)
				{
					Rsw::Object* o = map->getRsw()->objects[i];
					if (o->selected)
					{
						glm::vec3 diff = o->position - center;
						if (objectTranslateDirection != TranslatorTool::Axis::NONE && objectTranslateDirection != TranslatorTool::Axis::XYZ)
						{
							if (((int)objectTranslateDirection & (int)TranslatorTool::Axis::X) != 0)
								o->position.x -= lastmouse3d.x - mapRenderer.mouse3d.x;
							if (((int)objectTranslateDirection & (int)TranslatorTool::Axis::Y) != 0)
								o->position.y += (mouseState.position.y - lastMouseState.position.y) / 5.0f;
							if (((int)objectTranslateDirection & (int)TranslatorTool::Axis::Z) != 0)
								o->position.z += lastmouse3d.z - mapRenderer.mouse3d.z;
						}
						if (objectTranslateDirection == TranslatorTool::Axis::XYZ)
						{
							glm::vec3 diff = (glm::vec3(
								mapRenderer.mouse3d.x - 5 * map->getGnd()->width, 
								-mapRenderer.mouse3d.y, 
								10 + 5 * map->getGnd()->height - mapRenderer.mouse3d.z)) - center;
							o->position += diff;
						}
						if (objectRotateDirection != RotatorTool::Axis::NONE)
						{
							if (objectRotateDirection == RotatorTool::Axis::X)
							{
								glm::vec2 newPos(glm::distance(glm::vec2(o->position.y, o->position.z), glm::vec2(center.y, center.z)) *
									blib::math::fromAngle(glm::radians(glm::degrees(atan2(o->position.z - center.z, o->position.y - center.y)) + (mouseState.position.x - lastMouseState.position.x))));
								o->position.y = center.y + newPos.x;
								o->position.z = center.z + newPos.y;
								o->rotation.x += mouseState.position.x - lastMouseState.position.x;
							}
							if (objectRotateDirection == RotatorTool::Axis::Y)
							{
								glm::vec2 newPos(					glm::distance(glm::vec2(o->position.x, o->position.z), glm::vec2(center.x, center.z)) * 
									blib::math::fromAngle(glm::radians(glm::degrees(atan2(o->position.z - center.z, o->position.x - center.x))  + (mouseState.position.x - lastMouseState.position.x))));
								o->position.x = center.x + newPos.x;
								o->position.z = center.z + newPos.y;
								o->rotation.y -= mouseState.position.x - lastMouseState.position.x;
							}
							if (objectRotateDirection == RotatorTool::Axis::Z)
							{
								glm::vec2 newPos(glm::distance(glm::vec2(o->position.x, o->position.y), glm::vec2(center.x, center.y)) *
									blib::math::fromAngle(glm::radians(glm::degrees(atan2(o->position.y - center.y, o->position.x - center.x)) + (mouseState.position.x - lastMouseState.position.x))));
								o->position.x = center.x + newPos.x;
								o->position.y = center.y + newPos.y;
								o->rotation.z += mouseState.position.x - lastMouseState.position.x;
							}
						}
						if (objectScaleDirection != ScaleTool::Axis::NONE)
						{
							diff /= o->scale;
							if (objectScaleDirection == ScaleTool::Axis::X)
							{
								o->scale.x *= 1 - (mouseState.position.x - lastMouseState.position.x + mouseState.position.y - lastMouseState.position.y) * 0.01f;
								diff *= o->scale;
								o->position.x = center.x + diff.x;
							}
							if (objectScaleDirection == ScaleTool::Axis::Y)
							{
								o->scale.y *= 1 - (mouseState.position.x - lastMouseState.position.x + mouseState.position.y - lastMouseState.position.y) * 0.01f;
								diff *= o->scale;
								o->position.y = center.y + diff.y;
							}
							if (objectScaleDirection == ScaleTool::Axis::Z)
							{
								o->scale.z *= 1 - (mouseState.position.x - lastMouseState.position.x + mouseState.position.y - lastMouseState.position.y) * 0.01f;
								diff *= o->scale;
								o->position.z = center.z + diff.z;
							}
							if (objectScaleDirection == ScaleTool::Axis::ALL)
							{
								o->scale *= 1 + (mouseState.position.x - lastMouseState.position.x + mouseState.position.y - lastMouseState.position.y) * 0.01f;
								diff *= o->scale;
								o->position = center + diff;
							}
						}

						((Rsw::Model*)o)->matrixCached = false;
					}
				}
			}
		}

		if (!mouseState.rightButton && lastMouseState.rightButton && !mouseState.leftButton)
		{
			objectTranslateDirection = TranslatorTool::Axis::NONE;
			objectRotateDirection = RotatorTool::Axis::NONE;
			objectScaleDirection = ScaleTool::Axis::NONE;
			for (size_t i = 0; i < map->getRsw()->objects.size(); i++)
				map->getRsw()->objects[i]->selected = false;
		}
	}





	if (!wm->keyPressed)
	{
		if (keyState.isPressed(blib::Key::DEL) && !lastKeyState.isPressed(blib::Key::DEL))
		{
			for (int i = 0; i < (int)map->getRsw()->objects.size(); i++)
			{
				if (map->getRsw()->objects[i]->selected)
				{
					map->getRsw()->objects.erase(map->getRsw()->objects.begin() + i);
					i--;

					objectWindow->updateObjects(map);
				}
			}
		}
		if (keyState.isPressed(blib::Key::D) && !lastKeyState.isPressed(blib::Key::D))
		{
			int count = map->getRsw()->objects.size(); // array grows, so save it
			for (int i = 0; i < count; i++)
			{
				if (map->getRsw()->objects[i]->selected)
				{
					if (map->getRsw()->objects[i]->type == Rsw::Object::Type::Model)
					{
						addModel(((Rsw::Model*)map->getRsw()->objects[i])->fileName);

						map->getRsw()->objects[map->getRsw()->objects.size() - 1]->position = map->getRsw()->objects[i]->position + glm::vec3(10,0,10);
						map->getRsw()->objects[map->getRsw()->objects.size() - 1]->rotation = map->getRsw()->objects[i]->rotation;
						map->getRsw()->objects[map->getRsw()->objects.size() - 1]->scale = map->getRsw()->objects[i]->scale;

						map->getRsw()->objects[i]->selected = false;
						map->getRsw()->objects[map->getRsw()->objects.size() - 1]->selected = true;
					}
				}
			}
			newModel = NULL;
			objectWindow->updateObjects(map);
		}
	}
}
