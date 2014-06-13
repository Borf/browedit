uniform sampler2D s_texture;
uniform vec4 color;
uniform vec4 texMult;

varying vec2 texCoord;


void main()
{
	vec4 texColor = texMult * texture2D(s_texture, texCoord);

	if(color.a + texColor.a <= 0.01)
		discard;

	gl_FragData[0] = color + texColor;
	gl_FragData[1] = vec4(0,0,0,1);
}