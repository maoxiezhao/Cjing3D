
#define _DISABLE_OBJECT_PS_
#define _TRNAPARENT_

#include "..\hf\objectHF.hlsli"

void main(PixelInputTypePosTexColor input)
{
    float4 color;

    [branch]
    if (gMaterial.haveBaseColorMap > 0)
    {
        color = texture_basecolormap.Sample(sampler_linear_clamp, input.tex);
        color.rgb = DeGammaCorrect(color.rgb);
    }
    else
    {
        color = float4(1.0, 1.0, 1.0, 1.0);
    }
    color *= input.color;
    
    ALPHATEST(color.a);
}