attribute vec3 a_position;
attribute vec2 a_texcoord;

uniform mat4 projectionMatrix;
uniform mat4 modelviewMatrix;

varying vec2 texCoord;

void main()
{
	texCoord = a_texcoord;
	gl_Position = projectionMatrix * modelviewMatrix * vec4(a_position,1);
}