//
//  trans_math.cpp
//  metaltest5
//
//  Created by Color Yeti on 7/9/24.
//

#include "trans_math.hpp"
#include <math.h>

simd::float3 trans::add3(const simd::float3& A, const simd::float3& B)
{
    return {A.x+B.x, A.y+B.y, A.z+B.z};
}

simd::float4x4 trans::identity()
{
    const simd_float4 col0 = {1.0f, 0.0f, 0.0f, 0.0f};
    const simd_float4 col1 = {0.0f, 1.0f, 0.0f, 0.0f};
    const simd_float4 col2 = {0.0f, 0.0f, 1.0f, 0.0f};
    const simd_float4 col3 = {0.0f, 0.0f, 0.0f, 1.0f};
    return simd_matrix(col0, col1, col2, col3);
}

simd::float4x4 trans::perspective(float fov, float aspect, float znear, float zfar){
    float ys = 1.f / tanf(fov * 0.5f);
    float xs = ys / aspect;
    float zs = zfar / ( znear - zfar );
    const simd::float4 col0 = {xs, 0.0f, 0.0f, 0.0f};
    const simd::float4 col1 = {0.0f, ys, 0.0f, 0.0f};
    const simd::float4 col2 = {0.0f, 0.0f, zs, -1.0};
    const simd::float4 col3 = {0.0f, 0.0f, znear*zs, 0.0f};
    return simd_matrix(col0, col1, col2, col3);
}

simd::float4x4 trans::x_rotation(float theta){
    //theta = theta * M_PI / 180.0f;
    const float c = cosf(theta);
    const float s = sinf(theta);
    const simd_float4 col0 = {1.0f, 0.0f, 0.0f, 0.0f};
    const simd_float4 col1 = {0.0f, c , s , 0.0f};
    const simd_float4 col2 = {0.0f, -s, c, 0.0f};
    const simd_float4 col3 = {0.0f, 0.0f, 0.0f, 1.0f};
    return simd_matrix(col0, col1, col2, col3);
}

simd::float4x4 trans::y_rotation(float theta){
    //theta = theta * M_PI / 180.0f;
    const float c = cosf(theta);
    const float s = sinf(theta);
    const simd_float4 col0 = {c, 0.0f, -s, 0.0f};
    const simd_float4 col1 = {0.0f, 1.0f, 0.0f, 0.0f};
    const simd_float4 col2 = {s, 0.0f, c, 0.0f};
    const simd_float4 col3 = {0.0f, 0.0f, 0.0f, 1.0f};
    return simd_matrix(col0, col1, col2, col3);
}

simd::float4x4 trans::z_rotation(float theta){
    //theta = theta * M_PI / 180.0f;
    const float c = cosf(theta);
    const float s = sinf(theta);
    const simd_float4 col0 = {c, s, 0.0f, 0.0f};
    const simd_float4 col1 = {-s, c, 0.0f, 0.0f};
    const simd_float4 col2 = {0.0f, 0.0f, 1.0f, 0.0f};
    const simd_float4 col3 = {0.0f, 0.0f, 0.0f, 1.0f};
    return simd_matrix(col0, col1, col2, col3);
}

simd::float4x4 trans::translation(const simd::float3& dPos){
    const simd_float4 col0 = {1.0f, 0.0f, 0.0f, 0.0f};
    const simd_float4 col1 = {0.0f, 1.0f, 0.0f, 0.0f};
    const simd_float4 col2 = {0.0f, 0.0f, 1.0f, 0.0f};
    const simd_float4 col3 = {dPos.x, dPos.y, dPos.z, 1.0f};
    return simd_matrix(col0, col1, col2, col3);
}


simd::float4x4 trans::scale(const simd::float3& factor){
    const simd_float4 col0 = {factor.x, 0.0f, 0.0f, 0.0f};
    const simd_float4 col1 = {0.0f, factor.y, 0.0f, 0.0f};
    const simd_float4 col2 = {0.0f, 0.0f, factor.z, 0.0f};
    const simd_float4 col3 = {0.0f, 0.0f, 0.0f, 1.0f};
    return simd_matrix(col0, col1, col2, col3);
}

simd::float3x3 trans::discardTrans(const simd::float4x4& matrix){
    return simd_matrix(matrix.columns[0].xyz, matrix.columns[1].xyz, matrix.columns[2].xyz);
}
