#include "BrowEdit.h"
#include <BroLib/Map.h>
#include <BroLib/Gnd.h>

void BrowEdit::lightmapEditUpdate()
{
	if (keyState.isPressed(blib::Key::BRACKETLEFT) && !lastKeyState.isPressed(blib::Key::BRACKETLEFT))
		shadowMapColor -= 16;
	if (keyState.isPressed(blib::Key::BRACKETRIGHT) && !lastKeyState.isPressed(blib::Key::BRACKETRIGHT))
		shadowMapColor += 16;
	shadowMapColor = glm::clamp(shadowMapColor, 0, 256);

	if (keyState.isPressed(blib::Key::RIGHT) && !lastKeyState.isPressed(blib::Key::RIGHT))
		shadowMapBrush = (shadowMapBrush + 1) % shadowMapBrushes.size();
	if (keyState.isPressed(blib::Key::LEFT) && !lastKeyState.isPressed(blib::Key::LEFT))
		shadowMapBrush = (shadowMapBrush + (int)shadowMapBrushes.size() - 1) % shadowMapBrushes.size();

	if (mouseState.leftButton || lastMouseState.rightButton)
	{
		if (mouseState.position != lastMouseState.position || (mouseState.leftButton && !lastMouseState.leftButton) || (mouseState.rightButton && !lastMouseState.rightButton))
		{
			ShadowBrush& brush = shadowMapBrushes[shadowMapBrush];
			int h = brush.brush.size();
			int w = brush.brush[0].size();
			for (int y = 0; y < h; y++)
				for (int x = 0; x < w; x++)
					setLightmap(mapRenderer.mouse3d.x + x * 1 / 6.0f, mapRenderer.mouse3d.z + y * 1 / 6.0f, shadowMapColor, brush.brush[y][x].x);
		}
	}
}
