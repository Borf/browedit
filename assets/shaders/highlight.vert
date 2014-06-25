attribute vec3 a_position;
attribute vec2 a_texcoord;
attribute vec3 a_normal;

uniform mat4 projectionMatrix;
uniform mat4 modelviewMatrix;
uniform mat3 normalMatrix;

varying vec2 texCoord;
varying vec3 normal;

void main()
{
	texCoord = a_texcoord;
	normal = normalMatrix * a_normal;
	gl_Position = projectionMatrix * modelviewMatrix * vec4(a_position,1);
}