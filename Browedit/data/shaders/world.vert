#version 130

in vec3 a_position;
in vec2 a_texCoord1;
in vec2 a_texCoord2;
in vec3 a_normal;
in vec3 a_color;



uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat3 normalMatrix;

out vec2 texCoord;
out float nDotVP;

void main()
{
    vec3 eyeNormal = normalize(normalMatrix * a_normal);
    vec3 lightPosition = vec3(0.0, 0.0, 1.0);
    nDotVP = max(0.0, dot(eyeNormal, normalize(lightPosition)));
    texCoord = a_texCoord1;	
    gl_Position = modelMatrix * viewMatrix * projectionMatrix * vec4(a_position,1.0);
}
