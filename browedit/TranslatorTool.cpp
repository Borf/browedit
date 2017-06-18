#include "TranslatorTool.h"
#include "BrowEdit.h"
#include <blib/Shapes.h>
#include <blib/Shader.h>
#include <blib/Renderer.h>

#include <blib/linq.h>
#include <glm/gtc/matrix_transform.hpp>



TranslatorTool::TranslatorTool()
{
	//// build up arrow
	{
		int height = 20;
		int tip = 30;
		std::vector<blib::VertexP3> verts = blib::Shapes::box(glm::vec3(-2, 5, -2), glm::vec3(2, height, 2));
		verts.push_back(blib::VertexP3(glm::vec3(-5, height, -5)));
		verts.push_back(blib::VertexP3(glm::vec3(-5, height, 5)));
		verts.push_back(blib::VertexP3(glm::vec3(0, tip, 0)));

		verts.push_back(blib::VertexP3(glm::vec3(-5, height, 5)));
		verts.push_back(blib::VertexP3(glm::vec3(5, height, 5)));
		verts.push_back(blib::VertexP3(glm::vec3(0, tip, 0)));

		verts.push_back(blib::VertexP3(glm::vec3(5, height, 5)));
		verts.push_back(blib::VertexP3(glm::vec3(5, height, -5)));
		verts.push_back(blib::VertexP3(glm::vec3(0, tip, 0)));

		verts.push_back(blib::VertexP3(glm::vec3(5, height, -5)));
		verts.push_back(blib::VertexP3(glm::vec3(-5, height, -5)));
		verts.push_back(blib::VertexP3(glm::vec3(0, tip, 0)));
		for (size_t i = 0; i < verts.size(); i += 3)
		{
			glm::vec3 normal = glm::normalize(glm::cross(verts[i + 1].position - verts[i].position, verts[i + 2].position - verts[i].position));
			for (size_t ii = i; ii < i + 3; ii++)
				arrow.push_back(blib::VertexP3N3(verts[ii].position, normal));
		}
	}

}




void TranslatorTool::draw(const blib::math::Ray& mouseRay, blib::RenderState& highlightRenderState, const glm::vec3 &center, const glm::mat4 &modelView, blib::Renderer* renderer)
{
	Axis collides = selectedAxis(mouseRay, center);

	highlightRenderState.activeShader->setUniform(BrowEdit::HighlightShaderUniforms::texMult, glm::vec4(0, 0, 0, 0));
	highlightRenderState.activeTexture[0] = NULL;
	highlightRenderState.depthTest = true;


	glm::mat4 cameraMat = glm::translate(modelView, center);

	highlightRenderState.activeShader->setUniform(BrowEdit::HighlightShaderUniforms::modelviewMatrix, cameraMat);
	std::vector<blib::VertexP3> sheet(6);
	sheet[0] = blib::VertexP3(glm::vec3(0, 0, 0));
	sheet[1] = blib::VertexP3(glm::vec3(-10, 0, 0));
	sheet[2] = blib::VertexP3(glm::vec3(-10, 10, 0));
	sheet[3] = blib::VertexP3(glm::vec3(0, 0, 0));
	sheet[4] = blib::VertexP3(glm::vec3(0, 10, 0));
	sheet[5] = blib::VertexP3(glm::vec3(-10, 10, 0));
	highlightRenderState.activeShader->setUniform(BrowEdit::HighlightShaderUniforms::color, glm::vec4(1, 1, 0, collides == Axis::XY ? 1 : .25f));
	renderer->drawTriangles(sheet, highlightRenderState);

	sheet[0] = blib::VertexP3(glm::vec3(0, 0, 0));
	sheet[1] = blib::VertexP3(glm::vec3(-10, 0, 0));
	sheet[2] = blib::VertexP3(glm::vec3(-10, 0, 10));
	sheet[3] = blib::VertexP3(glm::vec3(0, 0, 0));
	sheet[4] = blib::VertexP3(glm::vec3(0, 0, 10));
	sheet[5] = blib::VertexP3(glm::vec3(-10, 0, 10));
	highlightRenderState.activeShader->setUniform(BrowEdit::HighlightShaderUniforms::color, glm::vec4(1, 0, 1, collides == Axis::XZ ? 1 : .25f));
	renderer->drawTriangles(sheet, highlightRenderState);

	sheet[0] = blib::VertexP3(glm::vec3(0, 0, 0));
	sheet[1] = blib::VertexP3(glm::vec3(0, 10, 0));
	sheet[2] = blib::VertexP3(glm::vec3(0, 10, 10));
	sheet[3] = blib::VertexP3(glm::vec3(0, 0, 0));
	sheet[4] = blib::VertexP3(glm::vec3(0, 0, 10));
	sheet[5] = blib::VertexP3(glm::vec3(0, 10, 10));
	highlightRenderState.activeShader->setUniform(BrowEdit::HighlightShaderUniforms::color, glm::vec4(0, 1, 1, collides == Axis::YZ ? 1 : .25f));
	renderer->drawTriangles(sheet, highlightRenderState);


	highlightRenderState.activeShader->setUniform(BrowEdit::HighlightShaderUniforms::diffuse, 0.75f);


	glm::mat4 finalModelView = glm::rotate(cameraMat, glm::radians(90.0f), glm::vec3(0, 0, 1));
	glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(finalModelView)));
	highlightRenderState.activeShader->setUniform(BrowEdit::HighlightShaderUniforms::modelviewMatrix, finalModelView);
	highlightRenderState.activeShader->setUniform(BrowEdit::HighlightShaderUniforms::normalMatrix, normalMatrix);
	highlightRenderState.activeShader->setUniform(BrowEdit::HighlightShaderUniforms::color, glm::vec4(1, 0, 0, collides == Axis::X ? 1 : 0.25f));
	renderer->drawTriangles(arrow, highlightRenderState);

	finalModelView = glm::rotate(cameraMat, glm::radians(0.0f), glm::vec3(1, 0, 0));
	normalMatrix = glm::transpose(glm::inverse(glm::mat3(finalModelView)));
	highlightRenderState.activeShader->setUniform(BrowEdit::HighlightShaderUniforms::modelviewMatrix, finalModelView);
	highlightRenderState.activeShader->setUniform(BrowEdit::HighlightShaderUniforms::normalMatrix, normalMatrix);
	highlightRenderState.activeShader->setUniform(BrowEdit::HighlightShaderUniforms::color, glm::vec4(0, 1, 0, collides == Axis::Y ? 1 : 0.25f));
	renderer->drawTriangles(arrow, highlightRenderState);

	finalModelView = glm::rotate(cameraMat, glm::radians(90.0f), glm::vec3(1, 0, 0));
	normalMatrix = glm::transpose(glm::inverse(glm::mat3(finalModelView)));
	highlightRenderState.activeShader->setUniform(BrowEdit::HighlightShaderUniforms::modelviewMatrix, finalModelView);
	highlightRenderState.activeShader->setUniform(BrowEdit::HighlightShaderUniforms::normalMatrix, normalMatrix);
	highlightRenderState.activeShader->setUniform(BrowEdit::HighlightShaderUniforms::color, glm::vec4(0, 0, 1, collides == Axis::Z ? 1 : 0.25f));
	renderer->drawTriangles(arrow, highlightRenderState);


	highlightRenderState.activeShader->setUniform(BrowEdit::HighlightShaderUniforms::diffuse, 0.0f);
	highlightRenderState.activeShader->setUniform(BrowEdit::HighlightShaderUniforms::modelviewMatrix, finalModelView);

}



TranslatorTool::Axis TranslatorTool::selectedAxis(const blib::math::Ray& mouseRay, const glm::vec3 &center)
{
	float t;
	blib::math::Ray transformedRay = mouseRay * glm::inverse(glm::rotate(glm::translate(glm::mat4(), center), glm::radians(90.0f), glm::vec3(0, 0, 1)));
	std::vector<glm::vec2> flatSheet;
	flatSheet.push_back(glm::vec2(0, 0));
	flatSheet.push_back(glm::vec2(0, 10));
	flatSheet.push_back(glm::vec2(10, 10));
	flatSheet.push_back(glm::vec2(10, 0));
	if (transformedRay.LineIntersectPolygon(blib::linq::reverse(blib::linq::select<std::vector<glm::vec3>>(flatSheet, [](glm::vec2 p) { return glm::vec3(p.x, p.y, 0); })), t))
		return Axis::XY;
	if (transformedRay.LineIntersectPolygon(blib::linq::select<std::vector<glm::vec3>>(flatSheet, [](glm::vec2 p) { return glm::vec3(0, p.x, p.y); }), t))
		return Axis::XZ;
	if (transformedRay.LineIntersectPolygon(blib::linq::reverse(blib::linq::select<std::vector<glm::vec3>>(flatSheet, [](glm::vec2 p) { return glm::vec3(p.x, 0, p.y); })), t))
		return Axis::YZ;

	blib::math::Ray transformedRayX = mouseRay * glm::inverse(glm::rotate(glm::translate(glm::mat4(), center), glm::radians(90.0f), glm::vec3(0, 0, 1)));
	blib::math::Ray transformedRayY = mouseRay * glm::inverse(glm::rotate(glm::translate(glm::mat4(), center), glm::radians(0.0f), glm::vec3(1, 0, 0)));
	blib::math::Ray transformedRayZ = mouseRay * glm::inverse(glm::rotate(glm::translate(glm::mat4(), center), glm::radians(90.0f), glm::vec3(1, 0, 0)));

	std::vector<glm::vec3> polygon(3);
	for (size_t i = 0; i < arrow.size(); i += 3)
	{
		for (size_t ii = 0; ii < 3; ii++)
			polygon[ii] = arrow[i + ii].position;
		if (transformedRayX.LineIntersectPolygon(polygon, t))
			return Axis::X;
		if (transformedRayY.LineIntersectPolygon(polygon, t))
			return Axis::Y;
		if (transformedRayZ.LineIntersectPolygon(polygon, t))
			return Axis::Z;
	}

	return TranslatorTool::Axis::NONE;
}