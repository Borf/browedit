attribute vec3 a_position;
attribute vec2 a_texture;
attribute vec2 a_texture2;
attribute vec3 a_color;
attribute vec3 a_normal;

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;

varying vec2 texCoord;
varying vec2 texCoord2;
varying vec3 color;

void main()
{
	texCoord = a_texture;
	texCoord2 = a_texture2;
	color = a_color;
	gl_Position = projectionMatrix * modelViewMatrix * vec4(a_position,1);
}