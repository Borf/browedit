uniform sampler2D Texture0, Texture1, Texture2;


void main()
{
	vec4 colorBase = texture2DProj(Texture0,gl_TexCoord[0]);
	vec4 colorColor = texture2DProj(Texture1,gl_TexCoord[1]);
	vec4 colorShadow = texture2DProj(Texture2,gl_TexCoord[1]);
	gl_FragColor = (colorBase * colorShadow * gl_Color) + colorColor;
}

/*
float4 GroundSpecularPixelShader(GroundPixelShaderInput input) : COLOR0
{
    float3 Normal = normalize(input.Normal);
    
    float4 Texture = tex2D( TextureSampler, input.TexCoord );
    clip(Texture.a - 0.5);
    
    float4 Shadowmap = tex2D( LightmapSampler, input.LightmapCoord );
    float3 Colormapc = tex2D( ColormapSampler, input.LightmapCoord );
    
    float DifIntensity = saturate(dot(Normal, Light)); 
    
    float4 AmbColor = float4(AmbientColor, 1);
    float4 DifColor = float4(DiffuseColor, 1);
    
    float4 Colormap = float4(Colormapc, 1);
	
    float4 result = Texture * AmbColor * AmbIntensity * Shadowmap
					+ Texture * DifColor * DifIntensity * Shadowmap
					+ Colormap;
	
    result.a = 1;
    return result;
}*/