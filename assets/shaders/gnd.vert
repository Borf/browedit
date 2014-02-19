attribute vec3 a_position;
attribute vec2 a_texture;

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;

varying vec2 texCoord;

void main()
{
	texCoord = a_texture;
	gl_Position = projectionMatrix * modelViewMatrix * vec4(a_position,1);
}