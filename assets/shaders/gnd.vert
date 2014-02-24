attribute vec3 a_position;
attribute vec2 a_texture;
attribute vec2 a_texture2;

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;

varying vec2 texCoord;
varying vec2 texCoord2;

void main()
{
	texCoord = a_texture;
	texCoord2 = a_texture2;
	gl_Position = projectionMatrix * modelViewMatrix * vec4(a_position,1);
}