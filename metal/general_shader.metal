//
//  general_shader.metal
//  Perspective
//
//  Created by Color Yeti on 2/2/25.
//

#import "general_shader.h"

v2f vertex vertexMain( device const VertexData* vertexData [[buffer(0)]],
                       device const InstanceData* instanceData [[buffer(1)]],
                       device const CameraData& cameraData [[buffer(2)]],
                       uint vertexId [[vertex_id]],
                       uint instanceId [[instance_id]] )
{
    v2f o;
    
    const device VertexData& vd = vertexData[vertexId];
    float4 pos = float4( vd.position, 1.0 );
    pos = instanceData[ instanceId ].instanceTransform * pos;
    pos = cameraData.perspectiveTransform * cameraData.worldTransform * pos;
    o.position = pos;
    
    float3 normal = instanceData[instanceId].instanceNormalTransform * vertexData[vertexId].norm;
    normal = cameraData.worldNormalTransform * normal ;
    o.norm = normal;
    
    o.texcoord = vd.texcoord.xy;
    
    o.color = half3( instanceData[ instanceId ].instanceColor.rgb );
    return o;
}

half4 fragment fragmentMain( v2f in [[stage_in]], texture2d<half, access::sample> tex[[texture(0)]] )
{
    constexpr sampler s( filter::linear);
    half4 textel = tex.sample(s, in.texcoord);
    half3 texel = textel.rgb;
    
    float3 l = normalize(float3(1.0,1.0, 0.8));
    float3 n = normalize(in.norm);
    
    half ndotl = half(saturate( dot(n, l) ));
    
    half3 illum = (in.color * 2.f) + (0.2f*in.color* ndotl);
    return half4( illum, textel.a);
}
