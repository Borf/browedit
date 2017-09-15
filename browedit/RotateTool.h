#pragma once

#include <vector>
#include <blib/gl/Vertex.h>
#include <blib/RenderState.h>
#include <blib/math/Ray.h>

namespace blib { class Renderer; };

class RotatorTool
{
	std::vector<blib::VertexP3N3> arrow;

public:
	enum class Axis
	{
		NONE = 0,
		X = 1,
		Y = 2,
		Z = 4,
	};

	RotatorTool();
	void draw(const blib::math::Ray& mouseRay, blib::RenderState& highlightRenderState, const glm::vec3 &center, const glm::mat4 &modelView, blib::Renderer* renderer, Axis activeAxis);
	Axis selectedAxis(const blib::math::Ray& mouseRay, const glm::vec3 &center);
};