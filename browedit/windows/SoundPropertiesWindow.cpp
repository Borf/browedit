#include "SoundPropertiesWindow.h"
#include "ObjectWindow.h"
#include "../BrowEdit.h"

#include <blib/wm/WM.h>
#include <blib/wm/widgets/textbox.h>
#include <blib/wm/widgets/button.h>
#include <blib/Util.h>

SoundPropertiesWindow::SoundPropertiesWindow(Rsw::Sound* sound, blib::ResourceManager* resourceManager, BrowEdit* browEdit) : blib::wm::Window("Sound Properties", "SoundPropertiesWindow.json", resourceManager)
{
	blib::wm::WM::getInstance()->center(this);

	getComponent<blib::wm::widgets::Textbox>("name")->text = sound->name;
	getComponent<blib::wm::widgets::Textbox>("filename")->text = sound->fileName;

	getComponent<blib::wm::widgets::Textbox>("positionx")->text = blib::util::toString(sound->position.x);
	getComponent<blib::wm::widgets::Textbox>("positiony")->text = blib::util::toString(sound->position.y);
	getComponent<blib::wm::widgets::Textbox>("positionz")->text = blib::util::toString(sound->position.z);

	getComponent<blib::wm::widgets::Textbox>("rotationx")->text = blib::util::toString(sound->rotation.x);
	getComponent<blib::wm::widgets::Textbox>("rotationy")->text = blib::util::toString(sound->rotation.y);
	getComponent<blib::wm::widgets::Textbox>("rotationz")->text = blib::util::toString(sound->rotation.z);

	getComponent<blib::wm::widgets::Textbox>("scalex")->text = blib::util::toString(sound->scale.x);
	getComponent<blib::wm::widgets::Textbox>("scaley")->text = blib::util::toString(sound->scale.y);
	getComponent<blib::wm::widgets::Textbox>("scalez")->text = blib::util::toString(sound->scale.z);
	
	/*
		std::string fileName;
		//float repeatDelay;
		float vol;
		long	width;
		long	height;
		float	range;
		float	cycle;


		char unknown6[8];
		float unknown7;
		float unknown8;*/

	getComponent<blib::wm::widgets::Textbox>("vol")->text = blib::util::toString(sound->vol);
	getComponent<blib::wm::widgets::Textbox>("width")->text = blib::util::toString(sound->width);
	getComponent<blib::wm::widgets::Textbox>("height")->text = blib::util::toString(sound->height);
	getComponent<blib::wm::widgets::Textbox>("range")->text = blib::util::toString(sound->range);
	getComponent<blib::wm::widgets::Textbox>("cycle")->text = blib::util::toString(sound->cycle);

	getComponent<blib::wm::widgets::Textbox>("unknown7")->text = blib::util::toString(sound->unknown7);
	getComponent<blib::wm::widgets::Textbox>("unknown8")->text = blib::util::toString(sound->unknown8);

	getComponent<blib::wm::widgets::Button>("btnCancel")->addClickHandler([this](int, int, int) { close(); return true;  });
	getComponent<blib::wm::widgets::Button>("btnOk")->addClickHandler([this, sound, browEdit](int, int, int)
	{ 
		sound->position.x = (float)atof(getComponent<blib::wm::widgets::Textbox>("positionx")->text.c_str());
		sound->position.y = (float)atof(getComponent<blib::wm::widgets::Textbox>("positiony")->text.c_str());
		sound->position.z = (float)atof(getComponent<blib::wm::widgets::Textbox>("positionz")->text.c_str());

		sound->rotation.x = (float)atof(getComponent<blib::wm::widgets::Textbox>("rotationx")->text.c_str());
		sound->rotation.y = (float)atof(getComponent<blib::wm::widgets::Textbox>("rotationy")->text.c_str());
		sound->rotation.z = (float)atof(getComponent<blib::wm::widgets::Textbox>("rotationz")->text.c_str());

		sound->scale.x = (float)atof(getComponent<blib::wm::widgets::Textbox>("scalex")->text.c_str());
		sound->scale.y = (float)atof(getComponent<blib::wm::widgets::Textbox>("scaley")->text.c_str());
		sound->scale.z = (float)atof(getComponent<blib::wm::widgets::Textbox>("scalez")->text.c_str());

		sound->name = getComponent<blib::wm::widgets::Textbox>("name")->text;
		sound->fileName = getComponent<blib::wm::widgets::Textbox>("fileName")->text;

		sound->vol = (float)atoi(getComponent<blib::wm::widgets::Textbox>("vol")->text.c_str());
		sound->width = atol(getComponent<blib::wm::widgets::Textbox>("width")->text.c_str());
		sound->height = atol(getComponent<blib::wm::widgets::Textbox>("height")->text.c_str());
		sound->range = (float)atof(getComponent<blib::wm::widgets::Textbox>("range")->text.c_str());
		sound->cycle = (float)atof(getComponent<blib::wm::widgets::Textbox>("cycle")->text.c_str());
		sound->unknown7 = (float)atof(getComponent<blib::wm::widgets::Textbox>("unknown7")->text.c_str());
		sound->unknown8 = (float)atof(getComponent<blib::wm::widgets::Textbox>("unknown8")->text.c_str());


		sound->matrixCached = false;
		close();

		browEdit->objectWindow->updateObjects(browEdit->map);

		return true;
	});

}

