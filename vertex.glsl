void main()
{
	gl_FrontColor = vec4(1,1,1,1);
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_TexCoord[1] = gl_MultiTexCoord1;
	gl_TexCoord[2] = gl_MultiTexCoord1;
	gl_Position = ftransform();
} 