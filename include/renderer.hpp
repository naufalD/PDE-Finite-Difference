#ifndef renderer_hpp
#define renderer_hpp

#include "config.h"
#include "mesh_factory.hpp"
#include "trans_math.hpp"

#include <stdio.h>

class Renderer{
public:
    Renderer( MTL::Device* device);
    ~Renderer();
    void buildMetal();
    void buildDepthStencilState();
    void generateTextures();
    void buildMeshes();
    void draw(MTK::View* view);
    MTL::RenderPipelineState* buildShader(const char* filename, const char* vertName, const char* fragName);
    MTL::ComputePipelineState* buildCompute(const char* filename, const char* computeName);

private:
    MTL::Device* _device;
    MTL::CommandQueue* commandQueue;
    MTL::CommandBuffer* _commandBuffer;
    MTL::RenderPipelineState *generalPipeline;//,*trianglePipeline;
    MTL::ComputePipelineState *computePipeline;
    MTL::DepthStencilState* m_depthStencilState;
    MTL::Buffer* m_textureAnimationBuffer;
    MTL::Texture* m_texture;
    float _angle;
    int _frame;
    uint _animationIndex;
    dispatch_semaphore_t m_semaphore;
    Mesh quadMesh;
    MeshFactory* TheMesh;
    
};

#endif /* renderer_hpp */
