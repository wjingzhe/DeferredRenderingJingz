cbuffer ConstantBuffer:register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
};


struct GBufferVertexOutput
{
	float4 Position:SV_POSITION;
	float3 Normal:NORMAL0;
	float2 TexCoord:TEXCOORD0;
	float2 Depth:TEXCOORD1;
};


struct GBufferPixelOutput
{
	half4 Color:COLOR0;
	half4 Normal:COLOR1;
	half4 Depth:COLOR2;
};

GBufferPixelOutput MainPS(GBufferVertexOutput input) : SV_TARGET
{
	GBufferPixelOutput output;
	output.Color = float4(1.0f, 1.0f, 1.0f, 1.0f);//tex2D.Sample(diffuseSampler,input.TexCoord);

	//float4 specularAttributes = tex2D.Sample(specularSample,input.TexCoord);

	//Specular intensity.
	output.Color.a = 0.0f;//specularAttributes.r;

	//Read the normal from the normal map //��ʱû��ʵ��
	float3 normalFromMap = input.Normal;//ԭ����û�н���ģ����ת����������ϵ�仯������ռ�ķ���û�з�������仯

	normalFromMap = normalize(normalFromMap);

	output.Normal.rgb = 0.5f*(normalFromMap + 1.0f);

	//Specular Power
	output.Normal.a = 1.0f;

	//Depth
	output.Depth = input.Depth.x / input.Depth.y;

	return output;

}