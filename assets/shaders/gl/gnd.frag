uniform sampler2D s_texture;
uniform sampler2D s_lighting;
uniform sampler2D s_tileColor;

uniform vec3 lightDiffuse;
uniform vec3 lightAmbient;
uniform vec3 lightDirection;
uniform float lightIntensity;

varying vec2 texCoord;
varying vec2 texCoord2;
varying vec2 tileColorCoord;
varying vec3 normal;



void main()
{
	vec4 texture = vec4(1,1,1,1);
	
	texture *= texture2D(s_texture, texCoord);
	texture *= texture2D(s_tileColor, tileColorCoord);
	texture *= texture2D(s_lighting, texCoord2).a;


	texture.rgb *= abs(dot(normal, lightDirection)) * lightDiffuse + lightIntensity * lightAmbient;

	texture += clamp(vec4(texture2D(s_lighting, texCoord2).rgb,1.0), 0.0, 1.0);


	gl_FragData[0] = texture;
	gl_FragData[1] = vec4(0,0,0,0);
}