#pragma once

namespace blib
{
	class Renderer;
}

class Renderer
{
public:
	virtual void render(blib::Renderer* renderer) = 0;

};