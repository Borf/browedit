#pragma once

#include <vector>
#include <blib/gl/Vertex.h>
#include <blib/RenderState.h>
#include <blib/math/Ray.h>

namespace blib { class Renderer; };

class ScaleTool
{
	std::vector<blib::VertexP3N3> arrow;

	std::vector<blib::VertexP3N3> cube;

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
		ALL = X|Y|Z,
	};

	ScaleTool();
	void draw(const blib::math::Ray& mouseRay, blib::RenderState& highlightRenderState, const glm::vec3 &center, const glm::mat4 &modelView, blib::Renderer* renderer);
	Axis selectedAxis(const blib::math::Ray& mouseRay, const glm::vec3 &center);
};