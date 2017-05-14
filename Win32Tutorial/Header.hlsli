struct VSIn
{
  float3 Position : POSITION0;
  float3 Color : COLOR0;
};

struct VSOut
{
  float4 Position : SV_POSITION;
  float3 Color : COLOR0;
};