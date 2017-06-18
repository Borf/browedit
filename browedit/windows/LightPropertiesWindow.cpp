#include "LightPropertiesWindow.h"
#include "ObjectWindow.h"
#include "../BrowEdit.h"

#include <blib/wm/WM.h>
#include <blib/wm/widgets/textbox.h>
#include <blib/wm/widgets/button.h>
#include <blib/Util.h>

#include <blib/wm/ActionMenuItem.h>
#include <blib/wm/Menu.h>

LightPropertiesWindow::LightPropertiesWindow(Rsw::Light* light, blib::ResourceManager* resourceManager, BrowEdit* browEdit) : blib::wm::Window("Model Properties", "LightPropertiesWindow.json", resourceManager)
{
	blib::wm::WM::getInstance()->center(this);

	getComponent<blib::wm::widgets::Textbox>("name")->text = light->name;

	getComponent<blib::wm::widgets::Textbox>("positionx")->text = blib::util::toString(light->position.x);
	getComponent<blib::wm::widgets::Textbox>("positiony")->text = blib::util::toString(light->position.y);
	getComponent<blib::wm::widgets::Textbox>("positionz")->text = blib::util::toString(light->position.z);

	getComponent<blib::wm::widgets::Textbox>("rotationx")->text = blib::util::toString(light->rotation.x);
	getComponent<blib::wm::widgets::Textbox>("rotationy")->text = blib::util::toString(light->rotation.y);
	getComponent<blib::wm::widgets::Textbox>("rotationz")->text = blib::util::toString(light->rotation.z);

	getComponent<blib::wm::widgets::Textbox>("scalex")->text = blib::util::toString(light->scale.x);
	getComponent<blib::wm::widgets::Textbox>("scaley")->text = blib::util::toString(light->scale.y);
	getComponent<blib::wm::widgets::Textbox>("scalez")->text = blib::util::toString(light->scale.z);

	getComponent<blib::wm::widgets::Textbox>("colorr")->text = blib::util::toString(light->color.r);
	getComponent<blib::wm::widgets::Textbox>("colorg")->text = blib::util::toString(light->color.g);
	getComponent<blib::wm::widgets::Textbox>("colorb")->text = blib::util::toString(light->color.b);

	getComponent<blib::wm::widgets::Textbox>("range")->text = blib::util::toString(light->range);
	getComponent<blib::wm::widgets::Textbox>("intensity")->text = blib::util::toString(light->intensity);
	getComponent<blib::wm::widgets::Textbox>("cutoff")->text = blib::util::toString(light->cutOff);


	getComponent<blib::wm::widgets::Button>("btnCancel")->addClickHandler([this](int, int, int) { close(); return true;  });
	getComponent<blib::wm::widgets::Button>("btnOk")->addClickHandler([this, light, browEdit](int, int, int)
	{ 
		light->position.x = (float)atof(getComponent<blib::wm::widgets::Textbox>("positionx")->text.c_str());
		light->position.y = (float)atof(getComponent<blib::wm::widgets::Textbox>("positiony")->text.c_str());
		light->position.z = (float)atof(getComponent<blib::wm::widgets::Textbox>("positionz")->text.c_str());

		light->rotation.x = (float)atof(getComponent<blib::wm::widgets::Textbox>("rotationx")->text.c_str());
		light->rotation.y = (float)atof(getComponent<blib::wm::widgets::Textbox>("rotationy")->text.c_str());
		light->rotation.z = (float)atof(getComponent<blib::wm::widgets::Textbox>("rotationz")->text.c_str());

		light->scale.x = (float)atof(getComponent<blib::wm::widgets::Textbox>("scalex")->text.c_str());
		light->scale.y = (float)atof(getComponent<blib::wm::widgets::Textbox>("scaley")->text.c_str());
		light->scale.z = (float)atof(getComponent<blib::wm::widgets::Textbox>("scalez")->text.c_str());

		light->name = getComponent<blib::wm::widgets::Textbox>("name")->text;

		light->range = (float)atof(getComponent<blib::wm::widgets::Textbox>("range")->text.c_str());
		light->intensity = (float)atof(getComponent<blib::wm::widgets::Textbox>("intensity")->text.c_str());
		light->cutOff = (float)atof(getComponent<blib::wm::widgets::Textbox>("cutoff")->text.c_str());


		light->matrixCached = false;
		close();

		//browEdit->objectWindow->updateObjects(browEdit->map);
		//dynamic_cast<blib::wm::ActionMenuItem*>(browEdit->rootMenu->getItem("Actions/Lightmaps/Calculate Lightmaps"))->callback();
		return true;
	});

}

