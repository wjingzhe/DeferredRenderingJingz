
Texture2D albedoTexture:register(t0);
Texture2D lightTexture:register(t1);

SamplerState samLinear:register(s0);

struct PS_INPUT
{
	float4 Pos:SV_POSITION;
	float2 Tex:TEXCOORD0;
};


float4 main(PS_INPUT input ) : SV_TARGET
{
	float3 diffuseColor = albedoTexture.Sample(samLinear,input.Tex).rgb;
	float4 light = lightTexture.Sample(samLinear, input.Tex);

	float3 diffuseLight = light.rgb;
	float specularLight = light.a;

	return float4((diffuseColor*diffuseLight + specularLight), 1.0f);
}