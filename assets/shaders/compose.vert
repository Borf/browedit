attribute vec2 a_position;
attribute vec2 a_texcoord;

varying vec2 texCoord;

void main()
{
	texCoord = a_texcoord;
	gl_Position.xy = a_position;
}