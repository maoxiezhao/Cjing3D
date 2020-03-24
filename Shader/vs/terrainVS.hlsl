#include "..\hf\objectInput.hlsli"
#include "..\hf\global.hlsli"

#define TERRAIN_TESSELATION

#ifdef TERRAIN_TESSELATION
struct HullInputType
{
    float4 pos      : POSITIONT;
    float2 tex      : TEXCOORD0;
    float4 color    : COLOR;
};

HullInputType main(InputObjectPos input)
{
    HullInputType Out;
    
    float4x4 worldMat = MakeWorldMatrixFromInstance(input.instance);
    VertexSurface surface = MakeVertexSurfaceFromInput(input);

    // scale positoin
    float cellX = input.instance.color.r;
    float cellZ = input.instance.color.g;
    float cellScale = input.instance.color.b;
    surface.position.x = (surface.position.x * cellScale) + cellX;
    surface.position.z = (surface.position.z * cellScale) + cellZ;
    
    float2 tex = float2(
        surface.position.x * gTerrainInverseResolution.x,
        surface.position.z * gTerrainInverseResolution.y
    );

    Out.pos = mul(surface.position, worldMat);
    Out.tex = tex;
    Out.color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    
    return Out;
}

#else 
// without tesselation

PixelInputType main(InputObjectPos input)
{
    PixelInputType Out;
    
    float4x4 worldMat = MakeWorldMatrixFromInstance(input.instance);
    VertexSurface surface = MakeVertexSurfaceFromInput(input);

    float2 tex = float2(
        surface.position.x * gTerrainInverseResolution.x,
        surface.position.z * gTerrainInverseResolution.y
    );
    
    float elevation = heightMap.SampleLevel(sampler_linear_clamp, tex, 0).r;
    elevation *= gTerrainElevation;
    
    float4 output = float4(
        surface.position.x,
        elevation,
        surface.position.z,
        1.0f);
    output = mul(output, worldMat);

    Out.pos3D = output;
    Out.pos = mul(gCameraVP, output);
    Out.tex = tex;
    Out.color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    
    // calcualte normal, 有限差分法
    float stepX = gTerrainInverseResolution.x;
    float stepY = gTerrainInverseResolution.y;
    float hl = heightMap.SampleLevel(sampler_linear_clamp, tex + float2(-stepX, 0), 0).r * gTerrainElevation;
    float hr = heightMap.SampleLevel(sampler_linear_clamp, tex + float2(stepX, 0), 0).r * gTerrainElevation;
    float ht = heightMap.SampleLevel(sampler_linear_clamp, tex + float2(0, stepY), 0).r * gTerrainElevation;
    float hb = heightMap.SampleLevel(sampler_linear_clamp, tex + float2(0, -stepY), 0).r * gTerrainElevation;
 
    Out.nor = normalize(float3(hl - hr, 2.0f, hb - ht));
    
    return Out;
}

#endif