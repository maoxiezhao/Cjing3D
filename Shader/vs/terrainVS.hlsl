#include "..\hf\objectInput.hlsli"
#include "..\hf\global.hlsli"

#define TERRAIN_TESSELATION

// *InputTerrainInstance.locTrans:
// x. x位移
// y. z位移
// z. 位移缩放

// *InputTerrainInstance.terrain:
// x. currentLodLevel
// y. edgeLodLevel
// z. cellVertexCount
struct InputTerrainInstance
{
    float4 locTrans : INSTANCELOCALTRANS;
    uint4 terrain : INSTANCETERRAIN;
};

struct InpuTerrainPos
{
    float4 pos : POSITION_NORMAL_SUBSETINDEX;
    
    InputTerrainInstance instance;
};

// InputTerrainInstance.terrain:
// x. currentLodLevel
// y. edgeLodLevel
// z. cellVertexCount
inline uint4 GetEdgeLodLevelFromInstance(InputTerrainInstance input, uint2 pos)
{
    uint currentLodLevel = input.terrain.x;
    uint vertexCount = input.terrain.z;
    uint4 ret = uint4(currentLodLevel, currentLodLevel, currentLodLevel, currentLodLevel);

    uint edgeLodLevel = input.terrain.y;
    
    // 仅仅对边界做细分    
    [branch]
    if (pos.x < 1) 
    { 
        ret.x = ((edgeLodLevel) & 0xff);
    }
    [branch]
    if (pos.x > vertexCount - 1)
    {
        ret.z = ((edgeLodLevel >> 16) & 0xff);
    }
    [branch]
    if (pos.y < 1)
    {
        ret.w = ((edgeLodLevel >> 24) & 0xff);
    }
    [branch]
    if (pos.y > vertexCount - 1)
    {
        ret.y = ((edgeLodLevel >> 8) & 0xff);
    }

    return ret;
}

inline VertexSurface MakeVertexSurfaceFromInput(InpuTerrainPos input)
{
    VertexSurface surface;
    surface.position = float4(input.pos.xyz, 1.0f);
    surface.color = gMaterial.baseColor;
    
    uint normalSubsetIndex = asuint(input.pos.w);
    surface.normal.x = (float) (normalSubsetIndex & 0x000000ff) / 255.0f * 2.0f - 1.0f;
    surface.normal.y = (float) ((normalSubsetIndex >> 8) & 0x000000ff) / 255.0f * 2.0f - 1.0f;
    surface.normal.z = (float) ((normalSubsetIndex >> 16) & 0x000000ff) / 255.0f * 2.0f - 1.0f;
    
    return surface;
}

struct HullInputType
{
    float4 pos      : POSITIONT;
    float2 tex      : TEXCOORD0;
    float4 color    : COLOR;
    uint4 edgeLevel : EDGELEVEL;
    uint2 terrainProp : TERRAINPROP;
};

HullInputType main(InpuTerrainPos input)
{
    HullInputType Out;
    VertexSurface surface = MakeVertexSurfaceFromInput(input);

    // scale positoin
    float cellX = input.instance.locTrans.r;
    float cellZ = input.instance.locTrans.g;
    float cellScale = input.instance.locTrans.b;
    float4 position = surface.position;
    
    position.x = (position.x * cellScale) + cellX;
    position.z = (position.z * cellScale) + cellZ;
    
    float2 tex = float2(
        position.x * gTerrainInverseResolution.x,
        position.z * gTerrainInverseResolution.y
    );

    Out.pos = mul(gTerrainTransform, position);
    Out.tex = tex;
    Out.color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    Out.terrainProp = input.instance.terrain.xz;
    
    uint2 localPos = uint2(surface.position.xz);
    Out.edgeLevel = GetEdgeLodLevelFromInstance(input.instance, localPos);
    
    return Out;
}