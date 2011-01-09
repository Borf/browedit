uniform sampler2D Texture0, Texture1, Texture2;


void main()
{
	vec4 colorBase = texture2DProj(Texture0,gl_TexCoord[0]);
	vec4 colorColor = texture2DProj(Texture1,gl_TexCoord[1]);
	vec4 colorShadow = texture2DProj(Texture2,gl_TexCoord[1]);
	gl_FragColor = (colorBase * colorShadow) + colorColor;
}