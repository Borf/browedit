#include "EffectPropertiesWindow.h"
#include "ObjectWindow.h"
#include "../BrowEdit.h"

#include <blib/wm/WM.h>
#include <blib/wm/widgets/textbox.h>
#include <blib/wm/widgets/button.h>
#include <blib/Util.h>

EffectPropertiesWindow::EffectPropertiesWindow(Rsw::Effect* effect, blib::ResourceManager* resourceManager, BrowEdit* browEdit) : blib::wm::Window("Model Properties", "EffectPropertiesWindow.json", resourceManager)
{
	blib::wm::WM::getInstance()->center(this);

	getComponent<blib::wm::widgets::Textbox>("name")->text = effect->name;
//	getComponent<blib::wm::widgets::Textbox>("model")->text = effect->fileName;

	getComponent<blib::wm::widgets::Textbox>("positionx")->text = blib::util::toString(effect->position.x);
	getComponent<blib::wm::widgets::Textbox>("positiony")->text = blib::util::toString(effect->position.y);
	getComponent<blib::wm::widgets::Textbox>("positionz")->text = blib::util::toString(effect->position.z);

	getComponent<blib::wm::widgets::Textbox>("rotationx")->text = blib::util::toString(effect->rotation.x);
	getComponent<blib::wm::widgets::Textbox>("rotationy")->text = blib::util::toString(effect->rotation.y);
	getComponent<blib::wm::widgets::Textbox>("rotationz")->text = blib::util::toString(effect->rotation.z);

	getComponent<blib::wm::widgets::Textbox>("scalex")->text = blib::util::toString(effect->scale.x);
	getComponent<blib::wm::widgets::Textbox>("scaley")->text = blib::util::toString(effect->scale.y);
	getComponent<blib::wm::widgets::Textbox>("scalez")->text = blib::util::toString(effect->scale.z);

	getComponent<blib::wm::widgets::Textbox>("effectId")->text = blib::util::toString(effect->id);
	getComponent<blib::wm::widgets::Textbox>("effectLoop")->text = blib::util::toString(effect->loop);
	getComponent<blib::wm::widgets::Textbox>("effectParam1")->text = blib::util::toString(effect->param1);
	getComponent<blib::wm::widgets::Textbox>("effectParam2")->text = blib::util::toString(effect->param2);
	getComponent<blib::wm::widgets::Textbox>("effectParam3")->text = blib::util::toString(effect->param3);
	getComponent<blib::wm::widgets::Textbox>("effectParam4")->text = blib::util::toString(effect->param4);

	getComponent<blib::wm::widgets::Button>("btnCancel")->addClickHandler([this](int, int, int) { close(); return true;  });
	getComponent<blib::wm::widgets::Button>("btnOk")->addClickHandler([this, effect, browEdit](int, int, int)
	{ 
		effect->position.x = (float)atof(getComponent<blib::wm::widgets::Textbox>("positionx")->text.c_str());
		effect->position.y = (float)atof(getComponent<blib::wm::widgets::Textbox>("positiony")->text.c_str());
		effect->position.z = (float)atof(getComponent<blib::wm::widgets::Textbox>("positionz")->text.c_str());

		effect->rotation.x = (float)atof(getComponent<blib::wm::widgets::Textbox>("rotationx")->text.c_str());
		effect->rotation.y = (float)atof(getComponent<blib::wm::widgets::Textbox>("rotationy")->text.c_str());
		effect->rotation.z = (float)atof(getComponent<blib::wm::widgets::Textbox>("rotationz")->text.c_str());

		effect->scale.x = (float)atof(getComponent<blib::wm::widgets::Textbox>("scalex")->text.c_str());
		effect->scale.y = (float)atof(getComponent<blib::wm::widgets::Textbox>("scaley")->text.c_str());
		effect->scale.z = (float)atof(getComponent<blib::wm::widgets::Textbox>("scalez")->text.c_str());

		effect->name = getComponent<blib::wm::widgets::Textbox>("name")->text;

		effect->id = (float)atoi(getComponent<blib::wm::widgets::Textbox>("effectId")->text.c_str());
		effect->loop = (float)atof(getComponent<blib::wm::widgets::Textbox>("effectLoop")->text.c_str());
		effect->param1 = (float)atof(getComponent<blib::wm::widgets::Textbox>("effectParam1")->text.c_str());
		effect->param2 = (float)atof(getComponent<blib::wm::widgets::Textbox>("effectParam2")->text.c_str());
		effect->param3 = (float)atof(getComponent<blib::wm::widgets::Textbox>("effectParam3")->text.c_str());
		effect->param4 = (float)atof(getComponent<blib::wm::widgets::Textbox>("effectParam4")->text.c_str());


		effect->matrixCached = false;
		close();

		browEdit->objectWindow->updateObjects(browEdit->map);

		return true;
	});

}

