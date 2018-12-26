#include "ColorWindow.h"
#include "../BrowEdit.h"
#include <BroLib/Map.h>

#include <blib/Util.h>
#include <blib/wm/WM.h>
#include <blib/wm/widgets/button.h>
#include <blib/wm/widgets/textbox.h>
#include <blib/util/Log.h>
using blib::util::Log;

#include "TextureWindow.h"
#include "ObjectWindow.h"
#include "MessageWindow.h"


ColorWindow::ColorWindow(blib::ResourceManager* resourceManager, BrowEdit* browEdit) : Window("Color", "ColorWindow.json", resourceManager)
{
	auto img = this->getComponent<blib::wm::widgets::Image>("hueslider");

	auto rgb = this->getComponent<blib::wm::widgets::Textbox>("rgb");
	auto hue = this->getComponent<blib::wm::widgets::Textbox>("hue");
	auto sat = this->getComponent<blib::wm::widgets::Textbox>("saturation");
	auto val = this->getComponent<blib::wm::widgets::Textbox>("value");
	img->addDragHandler([=](int x, int y)
	{
		float angle = (float)atan2(y - 128, x - 128);

		glm::vec3 hsv;
		hsv[0] = glm::mod(angle + glm::pi<float>() * 2.5f, glm::two_pi<float>()) / (glm::pi<float>() * 2) * 360;
		hsv[1] = glm::clamp(glm::length(glm::vec2(x - 128, y - 128)) / 128.0f, 0.0f, 1.0f);
		hsv[2] = (float)atof(val->text.c_str());
		this->color = blib::util::hsv2rgb(hsv);

		hue->text = std::to_string(hsv[0]);
		sat->text = std::to_string(hsv[1]);

		rgb->text = blib::util::rgb2hex(color);
		return true;
	});

	rgb->addChangeHandler([=]()
	{
		this->color = blib::util::hex2rgb(rgb->text);
		glm::vec3 hsv = blib::util::rgb2hsv(this->color);
		hue->text = std::to_string(hsv[0]);
		sat->text = std::to_string(hsv[1]);
		val->text = std::to_string(hsv[2]);
	});
	hue->addChangeHandler([=]()
	{
		glm::vec3 hsv = blib::util::rgb2hsv(this->color);
		hsv[0] = (float)atof(hue->text.c_str());
		this->color = blib::util::hsv2rgb(hsv);
		sat->text = std::to_string(hsv[1]);
		val->text = std::to_string(hsv[2]);
		rgb->text = blib::util::rgb2hex(color);
	});
	sat->addChangeHandler([=]()
	{
		glm::vec3 hsv = blib::util::rgb2hsv(this->color);
		hsv[1] = (float)atof(sat->text.c_str());
		this->color = blib::util::hsv2rgb(hsv);
		hue->text = std::to_string(hsv[0]);
		val->text = std::to_string(hsv[2]);
		rgb->text = blib::util::rgb2hex(color);
	});
	val->addChangeHandler([=]()
	{
		glm::vec3 hsv = blib::util::rgb2hsv(this->color);
		hsv[2] = (float)atof(val->text.c_str());
		this->color = blib::util::hsv2rgb(hsv);
		hue->text = std::to_string(hsv[0]);
		sat->text = std::to_string(hsv[1]);
		rgb->text = blib::util::rgb2hex(color);
	});


}

