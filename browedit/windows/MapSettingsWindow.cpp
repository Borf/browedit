#include "MapSettingsWindow.h"
#include "ObjectWindow.h"
#include "../BrowEdit.h"
#include <BroLib/Map.h>
#include <BroLib/Rsw.h>
#include <BroLib/Gat.h>
#include <BroLib/Gnd.h>

#include <blib/wm/WM.h>
#include <blib/wm/widgets/textbox.h>
#include <blib/wm/widgets/button.h>
#include <blib/Util.h>

MapSettingsWindow::MapSettingsWindow(blib::ResourceManager* resourceManager, BrowEdit* browEdit) : blib::wm::Window("Map Properties", "MapSettingsWindow.json", resourceManager)
{
	blib::wm::WM::getInstance()->center(this);


	getComponent<blib::wm::widgets::Textbox>("waterHeight")->text =			blib::util::toString(browEdit->map->getRsw()->water.height);
	getComponent<blib::wm::widgets::Textbox>("waterType")->text =			blib::util::toString(browEdit->map->getRsw()->water.type);
	getComponent<blib::wm::widgets::Textbox>("waterAmplitude")->text =		blib::util::toString(browEdit->map->getRsw()->water.amplitude);
	getComponent<blib::wm::widgets::Textbox>("waterPhase")->text =			blib::util::toString(browEdit->map->getRsw()->water.phase);
	getComponent<blib::wm::widgets::Textbox>("waterSurfaceCurve")->text =	blib::util::toString(browEdit->map->getRsw()->water.surfaceCurve);
	getComponent<blib::wm::widgets::Textbox>("waterAnimSpeed")->text =		blib::util::toString(browEdit->map->getRsw()->water.animSpeed);


	getComponent<blib::wm::widgets::Textbox>("lightLongitude")->text =		blib::util::toString(browEdit->map->getRsw()->light.longitude);
	getComponent<blib::wm::widgets::Textbox>("lightLatitude")->text =		blib::util::toString(browEdit->map->getRsw()->light.latitude);
	getComponent<blib::wm::widgets::Textbox>("lightDiffuseR")->text =		blib::util::toString(browEdit->map->getRsw()->light.diffuse.r);
	getComponent<blib::wm::widgets::Textbox>("lightDiffuseG")->text =		blib::util::toString(browEdit->map->getRsw()->light.diffuse.g);
	getComponent<blib::wm::widgets::Textbox>("lightDiffuseB")->text =		blib::util::toString(browEdit->map->getRsw()->light.diffuse.b);
	getComponent<blib::wm::widgets::Textbox>("lightAmbientR")->text =		blib::util::toString(browEdit->map->getRsw()->light.ambient.r);
	getComponent<blib::wm::widgets::Textbox>("lightAmbientG")->text =		blib::util::toString(browEdit->map->getRsw()->light.ambient.g);
	getComponent<blib::wm::widgets::Textbox>("lightAmbientB")->text =		blib::util::toString(browEdit->map->getRsw()->light.ambient.b);
	getComponent<blib::wm::widgets::Textbox>("lightIntensity")->text =		blib::util::toString(browEdit->map->getRsw()->light.intensity);

	getComponent<blib::wm::widgets::Textbox>("lightmapAmbient")->text = blib::util::toString(browEdit->map->getRsw()->light.lightmapAmbient);
	getComponent<blib::wm::widgets::Textbox>("lightmapIntensity")->text = blib::util::toString(browEdit->map->getRsw()->light.lightmapIntensity);


	getComponent<blib::wm::widgets::Button>("btnOk")->addClickHandler([this, browEdit](int, int, int) 
	{ 
		apply(browEdit);
		close();
		return true;
	});
	getComponent<blib::wm::widgets::Button>("btnApply")->addClickHandler([this, browEdit](int, int, int)
	{
		apply(browEdit);
		return true;
	});
	getComponent<blib::wm::widgets::Button>("btnCancel")->addClickHandler([this](int, int, int) { close(); return true;  });
}

void MapSettingsWindow::apply(BrowEdit* browEdit)
{
	browEdit->map->getRsw()->water.height =			atof(getComponent<blib::wm::widgets::Textbox>("waterHeight")->text.c_str());
	browEdit->map->getRsw()->water.type =			atoi(getComponent<blib::wm::widgets::Textbox>("waterType")->text.c_str());
	browEdit->map->getRsw()->water.amplitude =		atof(getComponent<blib::wm::widgets::Textbox>("waterAmplitude")->text.c_str());
	browEdit->map->getRsw()->water.phase =			atof(getComponent<blib::wm::widgets::Textbox>("waterPhase")->text.c_str());
	browEdit->map->getRsw()->water.surfaceCurve =	atof(getComponent<blib::wm::widgets::Textbox>("waterSurfaceCurve")->text.c_str());
	browEdit->map->getRsw()->water.animSpeed =		atoi(getComponent<blib::wm::widgets::Textbox>("waterAnimSpeed")->text.c_str());


	browEdit->map->getRsw()->light.longitude =		atoi(getComponent<blib::wm::widgets::Textbox>("lightLongitude")->text.c_str());
	browEdit->map->getRsw()->light.latitude =		atoi(getComponent<blib::wm::widgets::Textbox>("lightLatitude")->text.c_str());
	browEdit->map->getRsw()->light.diffuse.r =		atof(getComponent<blib::wm::widgets::Textbox>("lightDiffuseR")->text.c_str());
	browEdit->map->getRsw()->light.diffuse.g =		atof(getComponent<blib::wm::widgets::Textbox>("lightDiffuseG")->text.c_str());
	browEdit->map->getRsw()->light.diffuse.b =		atof(getComponent<blib::wm::widgets::Textbox>("lightDiffuseB")->text.c_str());
	browEdit->map->getRsw()->light.ambient.r =		atof(getComponent<blib::wm::widgets::Textbox>("lightAmbientR")->text.c_str());
	browEdit->map->getRsw()->light.ambient.g =		atof(getComponent<blib::wm::widgets::Textbox>("lightAmbientG")->text.c_str());
	browEdit->map->getRsw()->light.ambient.b =		atof(getComponent<blib::wm::widgets::Textbox>("lightAmbientB")->text.c_str());
	browEdit->map->getRsw()->light.intensity =		atof(getComponent<blib::wm::widgets::Textbox>("lightIntensity")->text.c_str());

}