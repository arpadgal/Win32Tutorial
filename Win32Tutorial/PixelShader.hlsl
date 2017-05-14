#include "Header.hlsli"

float4 main(VSOut input) : SV_TARGET
{
	return float4(input.Color.r, input.Color.g, input.Color.b, 1.0f);
}