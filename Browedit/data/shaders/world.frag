#version 130

uniform sampler2D s_texture;

in vec2 texCoord;
in float nDotVP;
out vec4 fragColor;

void main()
{
	vec4 col = texture2D(s_texture, texCoord);
    fragColor = col;
}
