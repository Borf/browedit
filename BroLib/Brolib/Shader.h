#pragma once

#include <string>
#include <list>
#include <map>

#include <gl/glew.h>
#include <gl/gl.h>
#include <glm/glm.hpp>

class Shader
{
	class SubShader
	{
		GLuint shaderId;
	public:
		enum ShaderType
		{
			Vertex,
			Fragment
		};


		SubShader(std::string filename, ShaderType shaderType);
		~SubShader();
		void attach(GLuint programId);
	};

	std::map<std::string, GLuint> uniformLocations;
	std::list<SubShader*> shaders;
	GLuint programId;
	 
	GLuint getUniformLocation(std::string name);

public:
	Shader(std::string vertex, std::string fragment);
	virtual ~Shader();

	void use();
	void link();

	void bindAttributeLocation(std::string name, int position);
	void bindFragLocation( std::string name, int position );

	void setUniform(std::string name,			const glm::mat4& value);
	void setUniform(std::string name,			const glm::mat3& value);
	void setUniform(std::string name,			const float& value);
	void setUniform(std::string name,			const int& value);
	void setUniform(std::string name,			const glm::vec4& value);
	void setUniform(std::string name,			const glm::vec3& value);

};

