attribute vec2 a_position;
attribute vec2 a_texcoord;

varying vec2 texCoord;

void main()
{
	texCoord = a_texcoord;
	gl_Position = vec4(a_position/2,0,0.5);
}