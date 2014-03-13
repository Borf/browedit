uniform sampler2D s_texture;

varying vec2 texCoord;


void main()
{
	vec4 color = texture2D(s_texture, texCoord);
	if(color.a < 0.1)
		discard;
	gl_FragColor = color;
}