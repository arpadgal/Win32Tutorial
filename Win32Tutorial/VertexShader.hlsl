#include "Header.hlsli"

VSOut main(VSIn input)
{
  VSOut wOutput;
  wOutput.Position = float4(input.Position, 1);
  wOutput.Color = input.Color;
	return wOutput;
}