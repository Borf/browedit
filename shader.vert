attribute vec4 a_position;
attribute vec2 a_texcoord;
attribute vec2 a_texcoord2;
attribute vec3 a_normal;
attribute vec4 a_color;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat3 normalMatrix;

varying vec2 texCoord;
varying vec2 texCoord2;

varying vec3 normal;
varying vec3 eyeVec;
varying vec3 lightDir;

void main()
{
    vec3 lightPosition = vec3(0.0, 100.0, 0.0);
    
    vec4 transformed = modelMatrix * viewMatrix * a_position;

    normal = normalMatrix * a_normal;
    lightDir = lightPosition-vec3(transformed);
    eyeVec = -vec3(transformed);

    texCoord = a_texcoord;
    texCoord2 = a_texcoord2;
    
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * a_position;
    
}
