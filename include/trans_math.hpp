//
//  trans_math.hpp
//  metaltest5
//
//  Created by Color Yeti on 7/9/24.
//

#ifndef trans_math_hpp
#define trans_math_hpp

#include <simd/simd.h>

namespace trans{
simd::float3 add3(const simd::float3& A, const simd::float3& B);
simd::float4x4 identity();
simd::float4x4 perspective(float fov, float aspect, float znear, float zfar);
simd::float4x4 x_rotation(float theta);
simd::float4x4 y_rotation(float theta);
simd::float4x4 z_rotation(float theta);
simd::float4x4 translation(const simd::float3& dPos);
simd::float4x4 scale(const simd::float3& factor);
simd::float3x3 discardTrans(const simd::float4x4& matrix);
}

#endif /* trans_math_hpp */
