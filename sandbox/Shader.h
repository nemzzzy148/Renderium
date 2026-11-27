//
// Created by Nemesis Verstraete on 27/11/2026.
//

#pragma once
#include <string>

const std::string slangShaderCode = R"(
struct VertexOutput
{
    float4 position : SV_Position;
    float3 color    : COLOR0;
};

// Vertex shader
[shader("vertex")]
VertexOutput vertMain(uint vertexID : SV_VertexID)
{
    // Hardcoded triangle vertices
    float2 positions[3] =
    {
        float2( 0.0,  0.5),
        float2( 0.5, -0.5),
        float2(-0.5, -0.5)
    };

    float3 colors[3] =
    {
        float3(1.0, 0.0, 0.0), // Red
        float3(0.0, 1.0, 0.0), // Green
        float3(0.0, 0.0, 1.0)  // Blue
    };

    VertexOutput output;

    output.position = float4(positions[vertexID], 0.0, 1.0);
    output.color = colors[vertexID];

    return output;
}

// Fragment shader
[shader("fragment")]
float4 fragMain(VertexOutput input) : SV_Target0
{
    return float4(input.color, 1.0);
}
)";
