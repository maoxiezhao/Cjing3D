
struct VertexToPixel
{
    float4 pos : SV_Position;
    float4 color : COLOR;
};

float4 main(VertexToPixel input) :  SV_TARGET
{
    return input.color;
}