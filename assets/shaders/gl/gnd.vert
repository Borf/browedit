attribute vec3 a_position;
attribute vec2 a_texture;
attribute vec2 a_texture2;
attribute vec2 a_tileColorCoord;
attribute vec3 a_normal;

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;

varying vec2 texCoord;
varying vec2 texCoord2;
varying vec2 tileColorCoord;
varying vec3 normal;

void main()
{
	texCoord = a_texture;
	texCoord2 = a_texture2;
	tileColorCoord = a_tileColorCoord;
	normal = a_normal;
	gl_Position = projectionMatrix * modelViewMatrix * vec4(a_position,1);
}