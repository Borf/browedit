uniform sampler2D s_texture;
uniform vec4 highlightColor;

uniform vec3 lightDiffuse;
uniform vec3 lightAmbient;
uniform vec3 lightDirection;
uniform float lightIntensity;

uniform int shadeType;


varying vec2 texCoord;
varying vec3 normal;




void main()
{
	vec4 color = texture2D(s_texture, texCoord);
	if(color.a < 0.1)
		discard;



	if(shadeType == 1 || shadeType == 2)
		color.rgb *= lightIntensity * clamp(dot(normalize(normal), lightDirection),0.0,1.0) * lightDiffuse + lightAmbient;

	if(shadeType == 4) // only for editor
		color.rgb *= lightDiffuse;

	gl_FragData[0] = color;
	gl_FragData[1] = highlightColor;
}