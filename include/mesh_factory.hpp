//
//  mesh_factory.hpp
//  metaltest3
//
//  Created by Color Yeti on 4/9/24.
//

#ifndef mesh_factory_hpp
#define mesh_factory_hpp

#include <stdio.h>
#include "config.h"

struct Mesh{
    MTL::Buffer* vertexBuffer, *indexBuffer, *instanceBuffer[kMaxFramesInFlight], *cameraBuffer[kMaxFramesInFlight];
};



class MeshFactory{
public:
    MeshFactory(MTL::Device* device, int height, int width);
    ~MeshFactory();
    void LaplaceCPU(int iterNum);
    void LaplaceGPU(MTL::CommandQueue* commandQueue, int iterNum);
    float getHeight(int x, int y);
    Mesh buildPlane(MTL::CommandBuffer* buffer, MTL::CommandQueue* commandQueue);
private:
    MTL::Device* m_device;
    MTL::ComputePipelineState* m_computePipeline;
    MTL::Buffer* m_widthBuffer;
    MTL::Buffer* m_initBuffer;
    MTL::Buffer* m_updatedBuffer;
    
    std::vector<float> m_initialHeight;
    std::vector<float> m_heightMap;
    int m_width;
    int m_height;
    
    void m_buildComputePipeline();
    void m_LaplaceGPU(MTL::CommandBuffer* buffer);

};

#endif /* mesh_factory_hpp */
