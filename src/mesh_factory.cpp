//
//  mesh_factory.cpp
//  metaltest3
//
//  Created by Color Yeti on 4/9/24.
//

#include "mesh_factory.hpp"
#include <chrono>

MeshFactory::MeshFactory(MTL::Device* device, int width, int height):m_device(device->retain())
{
    m_initialHeight.resize(width*height);
    m_heightMap.resize(width*height);
    
    for (int j=0; j<height; j++){
        for (int i=0; i<width; i++){
            float radius = sqrt((i-75)*(i-75)+(j-75)*(j-75));
            //if ((j==20) && 5<i && i<width-5){
            if (radius>8&&radius<9){
                m_initialHeight[i+j*width] = 100.f;
            }
            //else if ((j==30) && 5<i && i<width-5){
            else if(radius>10&&radius<12){
                m_initialHeight[i+j*width] = -100.f;
            }
            else{
                m_initialHeight[i+j*width] = 0.f;
            }
            m_heightMap[i+j*width] = m_initialHeight[i+j*width];
        }
    }
    
    
    m_width = width;
    m_height = height;
    m_buildComputePipeline();
}

MeshFactory::~MeshFactory()
{
    m_initBuffer->release();
    m_updatedBuffer->release();
    m_widthBuffer->release();
    m_computePipeline->release();
}

void MeshFactory::LaplaceCPU(int iterNum)
{
    for (int iter  = 0; iter<iterNum; iter++){
        for (int j = 1; j<(m_height-1); j++){
            for (int i = 1; i<(m_width-1); i++){
                
                if (m_initialHeight[i+j*m_width]==0){
                    m_heightMap[i+j*m_width] = 0.25*(m_heightMap[(i+1)+(j)*m_width]+m_heightMap[(i-1)+(j)*m_width]+m_heightMap[(i)+(j+1)*m_width]+m_heightMap[(i)+(j-1)*m_width]);
                }
//                else{
//                    m_heightMap[i+j*m_width] = m_initialHeight[i+j*m_width];
//                }
                
            }
        }
    }
}

void MeshFactory::LaplaceGPU(MTL::CommandQueue* commandQueue, int iterNum)
{
    for (int iter= 0; iter<iterNum; iter++){
        MTL::CommandBuffer* commandBuffer = commandQueue->commandBuffer();
        m_LaplaceGPU(commandBuffer);
    }
    memcpy(m_heightMap.data(), m_updatedBuffer->contents(), m_heightMap.size()*sizeof(float));
}

void MeshFactory::m_LaplaceGPU(MTL::CommandBuffer* buffer)
{
    assert(buffer);
    
    MTL::ComputeCommandEncoder* computeEncoder = buffer->computeCommandEncoder();
    
    computeEncoder->setComputePipelineState(m_computePipeline);
    computeEncoder->setBuffer(m_widthBuffer, 0, 3);
    computeEncoder->setBuffer(m_initBuffer, 0, 4);
    computeEncoder->setBuffer(m_updatedBuffer, 0, 5);
    
    MTL::Size gridSize = MTL::Size(m_width-1, m_height-1, 1);
    
    NS::UInteger threadGroupSize = m_computePipeline->maxTotalThreadsPerThreadgroup();
    MTL::Size threadgroupSize(threadGroupSize, 1, 1);
    
    
    computeEncoder->dispatchThreads(gridSize, threadgroupSize);
    computeEncoder->endEncoding();
    buffer->commit();
    buffer->waitUntilCompleted();
    
}

float MeshFactory::getHeight(int x, int y)
{
    return m_heightMap[x+y*m_width];
}

void MeshFactory::m_buildComputePipeline()
{
    NS::Error* error = nullptr;
    
    MTL::Library* computeLibrary = m_device->newDefaultLibrary();
    if (!computeLibrary){
        __builtin_printf( "%s", error->localizedDescription()->utf8String() );
        assert( false );
    }
    
    NS::String* computeName =NS::String::string("LaplaceGPU", NS::UTF8StringEncoding);
    MTL::Function* computeFunction = computeLibrary->newFunction(computeName);
    MTL::ComputePipelineState* pipeline = m_device->newComputePipelineState(computeFunction, &error);
    
    if (!pipeline)
    {
        __builtin_printf( "%s", error->localizedDescription()->utf8String() );
        assert( false );
    }
    
    computeFunction->release();
    computeLibrary->release();
    
    m_computePipeline = pipeline;
    
    const size_t widthSize = sizeof(m_width);
    const size_t initSize = m_initialHeight.size()*sizeof(float);
    const size_t updatedSize = m_heightMap.size()*sizeof(float);
    
    m_widthBuffer = m_device->newBuffer(widthSize, MTL::ResourceStorageModeShared);
    m_initBuffer = m_device->newBuffer(initSize, MTL::ResourceStorageModeShared);
    m_updatedBuffer = m_device->newBuffer(updatedSize, MTL::ResourceStorageModeShared);
    
    memcpy(m_widthBuffer->contents(), &m_width, widthSize);
    memcpy(m_initBuffer->contents(), m_initialHeight.data(), initSize);
    memcpy(m_updatedBuffer->contents(), m_heightMap.data(), updatedSize);
}

Mesh MeshFactory::buildPlane(MTL::CommandBuffer* buffer, MTL::CommandQueue* commandQueue)
{
    Mesh mesh;
    Vertex vertices[(m_width)*(m_height)];
    uint16_t indices[(m_width)*(m_height)*2+m_width];
    
    std::chrono::steady_clock::time_point cpuS = std::chrono::steady_clock::now();
    LaplaceCPU(100);
    std::chrono::steady_clock::time_point cpuE = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point gpuS = std::chrono::steady_clock::now();
    LaplaceGPU(commandQueue, 100);
    std::chrono::steady_clock::time_point gpuE = std::chrono::steady_clock::now();
    
    std::cout<<"Cpu Time: "<<std::chrono::duration_cast<std::chrono::microseconds>(cpuE-cpuS).count()<<"[us]"<<std::endl;
    std::cout<<"Gpu Time: "<<std::chrono::duration_cast<std::chrono::microseconds>(gpuE-gpuS).count()<<"[us]"<<std::endl;
    
    simd::float2 tex1 = { 0.f, 0.f };
    simd::float2 tex2 = { 0.f, 1.f };
    simd::float2 tex3 = { 1.f, 0.f };
    simd::float2 tex4 = { 1.f, 1.f };
    
    for (int j = 0; j<(m_height); j++){
        for (int i = 0; i<(m_width); i++){
            int index   = i + j * (m_width);
            int indexp1 = i + (j+1)*(m_width);
            
            float x = 1.f/m_width*(float)i-0.5;
            float y = 1.f/m_height*(float)j-0.5;
            //float z = height[index]/500;
            float z = m_heightMap[i+j*m_width]/500;
            
//            float normx =
            
            
            if (j%2 == 0){
                if (i%2 == 0){
                    vertices[index] = {{x, y, z}, {0.f, 0.f, 0.f}, tex1};
                }
                else if (i%2 == 1){
                    vertices[index] = {{x, y, z}, {0.f, 0.f, 0.f}, tex2};
                }
            }
            else if (j%2 == 1){
                if (i%2 == 0){
                    vertices[index] = {{x, y, z}, {0.f, 0.f, 0.f}, tex3};
                }
                else if (i%2 == 1){
                    vertices[index] = {{x, y,z}, {0.f, 0.f, 0.f}, tex4};
                }
            }
            
            if (j<m_height-1){
                indices[2*index+j] =  indexp1;
                indices[2*index+1+j] = index;
            }
            
        }
        indices[2*(m_width+j*m_height)+j] = 0xFFFF;
    }
    
    
    const size_t vertexDataSize = sizeof(vertices);
    const size_t indexDataSize = sizeof(indices);
    const size_t instanceDataSize =kMaxFramesInFlight*kNumInstances*sizeof(InstanceData);
    const size_t cameraDataSize =kMaxFramesInFlight*sizeof(CameraData);
    
    mesh.vertexBuffer = m_device->newBuffer(vertexDataSize, MTL::ResourceStorageModeShared);
    mesh.indexBuffer = m_device->newBuffer(indexDataSize, MTL::ResourceStorageModeShared);
    
    memcpy(mesh.vertexBuffer->contents(), vertices, vertexDataSize);
    memcpy(mesh.indexBuffer->contents(), indices, indexDataSize);
    
    
    for (size_t i = 0; i<kMaxFramesInFlight; ++i)
    {
        mesh.instanceBuffer[i] = m_device->newBuffer(instanceDataSize, MTL::ResourceStorageModeShared);
        mesh.cameraBuffer[i] = m_device->newBuffer(cameraDataSize, MTL::ResourceStorageModeShared);
    }
    
    return mesh;
}
