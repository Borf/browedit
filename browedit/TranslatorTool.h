#pragma once

#include <vector>
#include <blib/gl/Vertex.h>
#include <blib/RenderState.h>
#include <blib/math/Ray.h>

namespace blib { class Renderer; };

class TranslatorTool
{
	std::vector<blib::VertexP3N3> arrow;

public:
	enum class Axis
	{
		NONE = 0,
		X = 1,
		Y = 2,
		Z = 4,
		XY = X | Y,
		XZ = X | Z,
		YZ = Y | Z,
		XYZ = X | Y | Z,
	};

	TranslatorTool();
	void draw(const blib::math::Ray& mouseRay, blib::RenderState& highlightRenderState, const glm::vec3 &center, const glm::mat4 &modelView, blib::Renderer* renderer, Axis selectedAxis);
	Axis selectedAxis(const blib::math::Ray& mouseRay, const glm::vec3 &center);
};