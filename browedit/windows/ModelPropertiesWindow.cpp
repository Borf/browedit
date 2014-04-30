#include "ModelPropertiesWindow.h"

#include <blib/wm/WM.h>
#include <blib/wm/widgets/textbox.h>
#include <blib/wm/widgets/button.h>
#include <blib/Util.h>

ModelPropertiesWindow::ModelPropertiesWindow(Rsw::Model* model, blib::ResourceManager* resourceManager) : blib::wm::Window("Model Properties", "ModelPropertiesWindow.json", resourceManager)
{
	blib::wm::WM::getInstance()->center(this);

	getComponent<blib::wm::widgets::Textbox>("name")->text = model->name;
	getComponent<blib::wm::widgets::Textbox>("model")->text = model->fileName;

	getComponent<blib::wm::widgets::Textbox>("positionx")->text = blib::util::toString(model->position.x);
	getComponent<blib::wm::widgets::Textbox>("positiony")->text = blib::util::toString(model->position.y);
	getComponent<blib::wm::widgets::Textbox>("positionz")->text = blib::util::toString(model->position.z);

	getComponent<blib::wm::widgets::Textbox>("rotationx")->text = blib::util::toString(model->rotation.x);
	getComponent<blib::wm::widgets::Textbox>("rotationy")->text = blib::util::toString(model->rotation.y);
	getComponent<blib::wm::widgets::Textbox>("rotationz")->text = blib::util::toString(model->rotation.z);

	getComponent<blib::wm::widgets::Textbox>("scalex")->text = blib::util::toString(model->scale.x);
	getComponent<blib::wm::widgets::Textbox>("scaley")->text = blib::util::toString(model->scale.y);
	getComponent<blib::wm::widgets::Textbox>("scalez")->text = blib::util::toString(model->scale.z);



	getComponent<blib::wm::widgets::Button>("btnCancel")->addClickHandler([this](blib::wm::Widget*, int, int, int) { close();  });
	getComponent<blib::wm::widgets::Button>("btnOk")->addClickHandler([this, model](blib::wm::Widget*, int, int, int) 
	{ 
		model->position.x = atof(getComponent<blib::wm::widgets::Textbox>("positionx")->text.c_str());
		model->position.y = atof(getComponent<blib::wm::widgets::Textbox>("positiony")->text.c_str());
		model->position.z = atof(getComponent<blib::wm::widgets::Textbox>("positionz")->text.c_str());

		model->rotation.x = atof(getComponent<blib::wm::widgets::Textbox>("rotationx")->text.c_str());
		model->rotation.y = atof(getComponent<blib::wm::widgets::Textbox>("rotationy")->text.c_str());
		model->rotation.z = atof(getComponent<blib::wm::widgets::Textbox>("rotationz")->text.c_str());

		model->scale.x = atof(getComponent<blib::wm::widgets::Textbox>("scalex")->text.c_str());
		model->scale.y = atof(getComponent<blib::wm::widgets::Textbox>("scaley")->text.c_str());
		model->scale.z = atof(getComponent<blib::wm::widgets::Textbox>("scalez")->text.c_str());

		model->matrixCached = false;
		close();  
	});

}

