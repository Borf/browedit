#extension GL_EXT_gpu_shader4 : enable

uniform sampler2D s_texture;
uniform sampler2D s_texture2;

varying vec2 texCoord;


const int offset = 3;

void main()
{
	vec4 texColor = texture2D(s_texture, texCoord);
	
	
	vec4 texColor2 = 
		texture2DOffset(s_texture2, texCoord, ivec2(0.0,offset)) +
		texture2DOffset(s_texture2, texCoord, ivec2(0.0,-offset)) +
		texture2DOffset(s_texture2, texCoord, ivec2(offset,0.0)) +
		texture2DOffset(s_texture2, texCoord, ivec2(-offset,0.0)) -
		4.0 * texture2DOffset(s_texture2, texCoord, ivec2(0.0,0.0));


	float edge = round(max(max(abs(texColor2.r), abs(texColor2.g)), abs(texColor2.b)));


	vec4 fac = vec4(edge, -edge, -edge, 1.0);

	if(texColor.a <= 0.01)
		discard;
	gl_FragColor = clamp(texColor + fac, 0.0, 1.0);
}