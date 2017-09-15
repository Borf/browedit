#include "RotateTool.h"
#include "BrowEdit.h"
#include <blib/Shapes.h>
#include <blib/Shader.h>
#include <blib/Renderer.h>

#include <blib/linq.h>
#include <glm/gtc/matrix_transform.hpp>



ScaleTool::ScaleTool()
{
	//// build up arrow
	{
		int height = 20;
		std::vector<blib::VertexP3> verts = blib::Shapes::box(glm::vec3(-2, 5, -2), glm::vec3(2, height, 2));
		std::vector<blib::VertexP3> verts2 = blib::Shapes::box(glm::vec3(-5, height, -5), glm::vec3(5, height+10, 5));
		verts.insert(verts.end(), verts2.begin(), verts2.end());

		for (size_t i = 0; i < verts.size(); i += 3)
		{
			glm::vec3 normal = glm::normalize(glm::cross(verts[i + 1].position - verts[i].position, verts[i + 2].position - verts[i].position));
			for (size_t ii = i; ii < i + 3; ii++)
				arrow.push_back(blib::VertexP3N3(verts[ii].position, normal));
		}


		verts = blib::Shapes::box(glm::vec3(-5, -5, -5), glm::vec3(5, 5, 5));
		for (size_t i = 0; i < verts.size(); i += 3)
		{
			glm::vec3 normal = glm::normalize(glm::cross(verts[i + 1].position - verts[i].position, verts[i + 2].position - verts[i].position));
			for (size_t ii = i; ii < i + 3; ii++)
				cube.push_back(blib::VertexP3N3(verts[ii].position, normal));
		}

	}

}




void ScaleTool::draw(const blib::math::Ray& mouseRay, blib::RenderState& highlightRenderState, const glm::vec3 &center, const glm::mat4 &modelView, blib::Renderer* renderer, Axis activeAxis)
{
	Axis collides = selectedAxis(mouseRay, center);

	highlightRenderState.activeShader->setUniform(BrowEdit::HighlightShaderUniforms::texMult, glm::vec4(0, 0, 0, 0));
	highlightRenderState.activeTexture[0] = NULL;
	highlightRenderState.depthTest = true;


	glm::mat4 cameraMat = glm::translate(modelView, center);

	highlightRenderState.activeShader->setUniform(BrowEdit::HighlightShaderUniforms::diffuse, 0.75f);


	glm::mat4 finalModelView = glm::rotate(cameraMat, glm::radians(0.0f), glm::vec3(0, 0, 1)); // all axis
	glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(finalModelView)));
	highlightRenderState.activeShader->setUniform(BrowEdit::HighlightShaderUniforms::modelviewMatrix, finalModelView);
	highlightRenderState.activeShader->setUniform(BrowEdit::HighlightShaderUniforms::normalMatrix, normalMatrix);
	highlightRenderState.activeShader->setUniform(BrowEdit::HighlightShaderUniforms::color, glm::vec4(0.5f, 0.5f, 0.5f, collides == Axis::ALL ? 1 : 0.25f));
	renderer->drawTriangles(cube, highlightRenderState);



	finalModelView = glm::rotate(cameraMat, glm::radians(90.0f), glm::vec3(0, 0, 1)); // X axis
	normalMatrix = glm::transpose(glm::inverse(glm::mat3(finalModelView)));
	highlightRenderState.activeShader->setUniform(BrowEdit::HighlightShaderUniforms::modelviewMatrix, finalModelView);
	highlightRenderState.activeShader->setUniform(BrowEdit::HighlightShaderUniforms::normalMatrix, normalMatrix);
	highlightRenderState.activeShader->setUniform(BrowEdit::HighlightShaderUniforms::color, glm::vec4(1, 0, 0, collides == Axis::X ? 1 : 0.25f));
	renderer->drawTriangles(arrow, highlightRenderState);

	finalModelView = glm::rotate(cameraMat, glm::radians(0.0f), glm::vec3(1, 0, 0));			// y axis
	normalMatrix = glm::transpose(glm::inverse(glm::mat3(finalModelView)));
	highlightRenderState.activeShader->setUniform(BrowEdit::HighlightShaderUniforms::modelviewMatrix, finalModelView);
	highlightRenderState.activeShader->setUniform(BrowEdit::HighlightShaderUniforms::normalMatrix, normalMatrix);
	highlightRenderState.activeShader->setUniform(BrowEdit::HighlightShaderUniforms::color, glm::vec4(0, 1, 0, collides == Axis::Y ? 1 : 0.25f));
	renderer->drawTriangles(arrow, highlightRenderState);

	finalModelView = glm::rotate(cameraMat, glm::radians(90.0f), glm::vec3(1, 0, 0));			// z axis
	normalMatrix = glm::transpose(glm::inverse(glm::mat3(finalModelView)));
	highlightRenderState.activeShader->setUniform(BrowEdit::HighlightShaderUniforms::modelviewMatrix, finalModelView);
	highlightRenderState.activeShader->setUniform(BrowEdit::HighlightShaderUniforms::normalMatrix, normalMatrix);
	highlightRenderState.activeShader->setUniform(BrowEdit::HighlightShaderUniforms::color, glm::vec4(0, 0, 1, collides == Axis::Z ? 1 : 0.25f));
	renderer->drawTriangles(arrow, highlightRenderState);


	highlightRenderState.activeShader->setUniform(BrowEdit::HighlightShaderUniforms::diffuse, 0.0f);
	highlightRenderState.activeShader->setUniform(BrowEdit::HighlightShaderUniforms::modelviewMatrix, finalModelView);

}



ScaleTool::Axis ScaleTool::selectedAxis(const blib::math::Ray& mouseRay, const glm::vec3 &center)
{
	float t;


	blib::math::Ray transformedRayX = mouseRay * glm::inverse(glm::rotate(glm::translate(glm::mat4(), center), glm::radians(90.0f), glm::vec3(0, 0, 1)));
	blib::math::Ray transformedRayY = mouseRay * glm::inverse(glm::rotate(glm::translate(glm::mat4(), center), glm::radians(0.0f), glm::vec3(1, 0, 0)));
	blib::math::Ray transformedRayZ = mouseRay * glm::inverse(glm::rotate(glm::translate(glm::mat4(), center), glm::radians(90.0f), glm::vec3(1, 0, 0)));

	std::vector<glm::vec3> polygon(3);
	for (size_t i = 0; i < cube.size(); i += 3)
	{
		for (size_t ii = 0; ii < 3; ii++)
			polygon[ii] = cube[i + ii].position;
		if (transformedRayY.LineIntersectPolygon(polygon, t))
			return Axis::ALL;
	}

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

	return Axis::NONE;
}