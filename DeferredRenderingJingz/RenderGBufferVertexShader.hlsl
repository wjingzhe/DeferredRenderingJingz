cbuffer ConstantBuffer:register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
}

struct GBufferVertexInput
{
	float3 Position:POSITION0;
	float2 TexCoord:TEXCOORD0;
	float3 Normal:NORMAL0;

	matrix InstancedWorld : WORLD;

	//part3为自动生成
	uint InstanceId : SV_InstanceID;
};

struct GBufferVertexOutput
{
	float4 Position:SV_POSITION;
	float3 Normal:NORMAL0;
	float2 TexCoord:TEXCOORD0;
	float2 Depth:TEXCOORD1;
};

GBufferVertexOutput MainVS(GBufferVertexInput input)
{
	GBufferVertexOutput output;
	float4 worldPosition = mul(float4(input.Position.xyz, 1.0f), input.InstancedWorld);
	float4 viewPosition = mul(worldPosition, View);
	output.Position = mul(viewPosition, Projection);
	output.Normal = input.Normal;
	output.TexCoord = input.TexCoord;
	output.Depth.x = output.Position.z;
	output.Depth.y = output.Position.w;


	return output;
}