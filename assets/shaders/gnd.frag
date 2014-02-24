uniform sampler2D s_texture;
uniform sampler2D s_lighting;

varying vec2 texCoord;
varying vec2 texCoord2;


void main()
{
	float shadow = texture2D(s_lighting, texCoord2).a;
	gl_FragColor = texture2D(s_texture, texCoord) * vec4(shadow,shadow,shadow,1);
}