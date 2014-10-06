uniform sampler2D s_texture;
uniform vec4 color;
uniform vec4 texMult;

uniform float diffuse;

varying vec2 texCoord;
varying vec3 normal;

void main()
{
	vec4 texColor = texMult * texture2D(s_texture, texCoord);

	if(color.a + texColor.a <= 0.01)
		discard;

	float diffuseFac = abs(dot(normal, vec3(0,0,1)));


	gl_FragData[0] = mix(color + texColor, (color + texColor) * vec4(diffuseFac,diffuseFac,diffuseFac,1), diffuse);
	gl_FragData[1] = vec4(0,0,0,1);
}