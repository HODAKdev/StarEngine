float3 To_ACES(float3 linearColor)
{
	float a = 2.51f;
	float b = 0.03f;
	float c = 2.43f;
	float d = 0.59f;
	float e = 0.14f;
    return saturate((linearColor * (a * linearColor + b)) / (linearColor * (c * linearColor + d) + e));
}