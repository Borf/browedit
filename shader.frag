uniform sampler2D s_texture;
varying vec2 texCoord;

varying vec3 normal;
varying vec3 eyeVec;
varying vec3 lightDir;

void main()
{
    gl_FragColor = vec4(1,0,0,1);
    return;
    
    vec3 N = normalize(normal);
    vec3 L = normalize(lightDir);
    float nDotVP = dot(N, L);

    float specular = 0.0;

    if (nDotVP > 0.0)
    {
        vec3 E = normalize(eyeVec);
        vec3 R = reflect(-L, N);
        specular =  pow( max(dot(R, E), 0.0), 10.0 );
    }

    float ambient = 0.2;
    float diffuse = max(0.0, nDotVP);

    vec4 col = (ambient + specular + diffuse) * texture2D(s_texture, texCoord);
    gl_FragColor = col;
}
