#include "World.h"
#include <vector>

World::World(std::string fileName)
{
	std::vector<VertexPosition> verts;

	verts.push_back(VertexPosition(glm::vec3(-10,0,-10)));
	verts.push_back(VertexPosition(glm::vec3(10,0,-10)));
	verts.push_back(VertexPosition(glm::vec3(10,10,-10)));

	vbo.setData(3, &(*verts.begin()), GL_STATIC_DRAW);

	vao = new VAO<VertexPosition>(&vbo);

}


World::~World(void)
{

}


void World::draw(Camera* camera, WorldShader* shader)
{
	shader->use();
	shader->setViewMatrix(camera->getMatrix());
	shader->setModelMatrix(glm::mat4());


	vao->bind();


	glDrawArrays(GL_TRIANGLES, 0, 3);

	vao->unBind();

}