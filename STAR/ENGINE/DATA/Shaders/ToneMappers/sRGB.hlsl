float3 To_sRGB(float3 linearColor)
{
    return pow(linearColor.rgb, 1.0/2.2);
}