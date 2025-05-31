//
//  config.h
//  metaltest
//
//  Created by Color Yeti on 1/9/24.
//

#ifndef config_h
#define config_h

#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <AppKit/AppKit.hpp>
#include <MetalKit/MetalKit.hpp>
#include <simd/simd.h>

#include <iostream>
#include <fstream>
#include <sstream>

static constexpr size_t kSphereSize = 150;
static constexpr size_t kMaxFramesInFlight = 3;
static constexpr size_t kInstanceRows = 1;
static constexpr size_t kInstanceColumns = 1;
static constexpr size_t kInstanceDepth = 1;
static constexpr size_t kNumInstances = (kInstanceRows * kInstanceDepth * kInstanceColumns);
static constexpr uint32_t kTextureWidth = 64;
static constexpr uint32_t kTextureHeight = 64;

struct Vertex{
    simd::float4 pos;
    simd::float4 normal;
    simd::float2 texcoord;
};

struct InstanceData{
    simd::float4x4 instanceTransform;
    simd::float4 instanceColor;
    simd::float3x3 instanceNormalTransform;
};

struct CameraData{
    simd::float4x4 perspectiveTransform;
    simd::float4x4 worldTransform;
    simd::float3x3 worldNormalTransform;
};


#endif /* config_h */
