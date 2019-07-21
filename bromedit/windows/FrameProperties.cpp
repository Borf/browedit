#include "FrameProperties.h"

#include <BroLib/MapRenderer.h>
#include <blib/ResourceManager.h>
#include <blib/wm/widgets/textbox.h>

FrameProperties::FrameProperties(blib::ResourceManager * resourceManager) : blib::wm::Window("Frame Properties", "FramePropertiesWindow.json", resourceManager)
{
	addHandler("time",	[](Rsm::Mesh::Frame* frame) { return &frame->time; });
	addHandler("quatx",	[](Rsm::Mesh::Frame* frame) { return &frame->quaternion[0]; });
	addHandler("quaty",	[](Rsm::Mesh::Frame* frame) { return &frame->quaternion[1]; });
	addHandler("quatz",	[](Rsm::Mesh::Frame* frame) { return &frame->quaternion[2]; });
	addHandler("quatw",	[](Rsm::Mesh::Frame* frame) { return &frame->quaternion[3]; });


}



void FrameProperties::selectFrame(Rsm::Mesh::Frame* frame)
{
	selectedFrame = frame;

	if (frame != nullptr)
	{
		for (auto& c : intCallbacks)
			c.first->text = blib::util::toString(*c.second(frame));

		for (auto& c : floatCallbacks)
			c.first->text = blib::util::toString(*c.second(frame));
	}
	else
	{
		for (auto& c : intCallbacks)
			c.first->text = "";

		for (auto& c : floatCallbacks)
			c.first->text = "";
	}
}

void FrameProperties::addHandler(const std::string & name, std::function<int*(Rsm::Mesh::Frame*)> callback)
{
	auto textbox = this->getComponent<blib::wm::widgets::Textbox>(name);
	textbox->addChangeHandler([this,textbox, callback]()
	{
		*callback(selectedFrame) = atoi(textbox->text.c_str());
	});
	intCallbacks.push_back(std::pair<blib::wm::widgets::Textbox*, std::function<int*(Rsm::Mesh::Frame*)>>(textbox, callback));
}

void FrameProperties::addHandler(const std::string & name, std::function<float*(Rsm::Mesh::Frame*)> callback)
{
	auto textbox = this->getComponent<blib::wm::widgets::Textbox>(name);
	textbox->addChangeHandler([this, textbox, callback]()
	{
		if (selectedFrame != nullptr)
		{
			*callback(selectedFrame) = (float)atof(textbox->text.c_str());
		}
	});
	floatCallbacks.push_back(std::pair<blib::wm::widgets::Textbox*, std::function<float*(Rsm::Mesh::Frame*)>>(textbox, callback));
}
