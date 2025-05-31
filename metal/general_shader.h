//
//  general_shader.h
//  Compute
//
//  Created by Color Yeti on 3/5/25.
//
#ifndef general_shader
#define general_shader

#include <metal_stdlib>
using namespace metal;

struct v2f
{
    float4 position [[position]];
    float3 norm;
    half3 color;
    float2 texcoord;
};

struct VertexData
{
    float3 position;
    float3 norm;
    float2 texcoord;
};

struct InstanceData
{
    float4x4 instanceTransform;
    float3x3 instanceNormalTransform;
    float4 instanceColor;
};

struct CameraData
{
    float4x4 perspectiveTransform;
    float4x4 worldTransform;
    float3x3 worldNormalTransform;
};

#endif
