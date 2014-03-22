uniform sampler2D s_texture;
uniform vec4 color;
uniform vec4 texMult;

varying vec2 texCoord;


void main()
{
	vec4 texColor = texMult * texture2D(s_texture, texCoord);
	gl_FragColor = color + texColor;
}