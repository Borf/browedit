uniform sampler2D s_texture;
uniform sampler2D s_lighting;

varying vec2 texCoord;
varying vec2 texCoord2;


void main()
{
	float shadow = texture2D(s_lighting, texCoord2).a;
	vec4 light = vec4(texture2D(s_lighting, texCoord2).rgb,1);

	vec4 final = texture2D(s_texture, texCoord) * vec4(shadow,shadow,shadow,1) + light;

	gl_FragData[0] = final;
	gl_FragData[1] = vec4(0,0,0,0);
}