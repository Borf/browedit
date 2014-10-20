uniform sampler2D s_texture;
uniform sampler2D s_lighting;

varying vec2 texCoord;
varying vec2 texCoord2;
varying vec3 color;


void main()
{
	vec4 texture = texture2D(s_texture, texCoord);

	texture *= vec4(color,1);
	texture *= texture2D(s_lighting, texCoord2).a;
	texture += vec4(texture2D(s_lighting, texCoord2).rgb,1);


	gl_FragData[0] = texture;
	gl_FragData[1] = vec4(0,0,0,0);
}