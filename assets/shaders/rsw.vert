attribute vec3 a_position;
attribute vec2 a_texture;
attribute vec3 a_normal;

uniform mat4 projectionMatrix;
uniform mat4 cameraMatrix;
uniform mat4 modelMatrix;
uniform mat4 modelMatrix2;
uniform float billboard;

varying vec2 texCoord;

void main()
{
	texCoord = a_texture;
	vec4 billboarded = projectionMatrix * (cameraMatrix * modelMatrix) * vec4(0.0,0.0,0.0,1.0) +  modelMatrix2 * vec4(a_position.x, a_position.y,0.0,1.0);
	vec4 position = projectionMatrix * cameraMatrix * modelMatrix2 * modelMatrix * vec4(a_position,1.0);

	gl_Position = mix(position, billboarded, billboard);
}