#include "..\hf\imageHF.hlsli"

struct SpriteInputInstance
{
    // per Instance
    // {xy__} = position.xy
    // {__zw} = scale.xy
    float4 Translation         : TRANSLATION;

    // per Instance
    // {xy__} = {rect.xy}
    // {__zw} = {rect.width, rect.height}
    float4 SourceRect          : BINORMAL;

    // per Instance
    // {xy__} = originPivot.xy
    // {__z_} = rotation
    // {___w} = layerDepth
    float4 OriginRotationDepth : NORMAL;

    // per Instance
    // {xyzw} = color.rgba
    float4 Color               : COLOR;

    // per Instance
    // {xy__} = {1.0f / textureWidth, 1.0f / textureHeight}
    // {__z_} = unused
    // {___w} = unused
    float4 InverseTextureSize  : TEXCOORD0;
};

struct SpriteInputPosTex
{
    float4 pos : POSITION_TEXCOORD;
    
    SpriteInputInstance instance;
};

float3x3 MakeWorldMatrixFromInstance(SpriteInputInstance input)
{
    float3x3 scaling = float3x3(
        float3(input.Translation.z * input.SourceRect.z, 0.0f, 0.0f),
        float3(0.0f, input.Translation.w * input.SourceRect.w, 0.0f),
        float3(0.0f, 0.0f, 1.0f));

    float cosRotation = cos(input.OriginRotationDepth.z);
    float sinRotation = sin(input.OriginRotationDepth.z);
    float3x3 rotate = float3x3(
        float3(cosRotation, sinRotation, 0.0f),
        float3(-sinRotation, cosRotation, 0.0f),
        float3(0.0f, 0.0f, 1.0f));

    float3x3 translate = float3x3(
        float3(1.0f, 0.0f, 0.0f),
        float3(0.0f, 1.0f, 0.0f),
        float3(input.Translation.xy, 1.0f));

    return mul(mul(scaling, rotate), translate);
}

SpritePixelInput main(SpriteInputPosTex input)
{
    float3x3 worldMat = MakeWorldMatrixFromInstance(input.instance);
    float3 position = float3(input.pos.xy - input.instance.OriginRotationDepth.xy, 1.0f);
    position = mul(position, worldMat);
    float4 finalPos = mul(gSpriteCameraVP, float4(position.xy, 0.0f, 1.0f));
    
    SpritePixelInput Out;
    Out.pos = float4(finalPos.xy, input.instance.OriginRotationDepth.w, 1);
    Out.uv = (input.pos.zw * input.instance.SourceRect.zw + input.instance.SourceRect.xy) * input.instance.InverseTextureSize.xy;
    Out.color = input.instance.Color;
    
    return Out;
}
