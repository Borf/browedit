uniform sampler2D s_texture;
uniform vec4 highlightColor;

varying vec2 texCoord;


void main()
{
	vec4 color = texture2D(s_texture, texCoord);
	if(color.a < 0.1)
		discard;
	gl_FragData[0] = color;
	gl_FragData[1] = highlightColor;
}