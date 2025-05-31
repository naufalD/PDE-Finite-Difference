//
//  renderer.cpp
//  Perspective
//
//  Created by Color Yeti on 1/2/25.
//

#include "renderer.hpp"
#include <unistd.h>
Renderer::Renderer(MTL::Device* device)
:_device(device->retain())
, _angle (0.f)
, _frame (0)
, _animationIndex(0)
{
    commandQueue = _device->newCommandQueue();
    _commandBuffer = commandQueue->commandBuffer();
    buildMeshes();
    buildMetal();
    buildDepthStencilState();
    generateTextures();
    
    m_textureAnimationBuffer = _device->newBuffer(sizeof(uint), MTL::ResourceStorageModeShared);

    m_semaphore = dispatch_semaphore_create(kMaxFramesInFlight);
}

Renderer::~Renderer()
{
    m_depthStencilState->release();
    generalPipeline->release();
    commandQueue->release();
    _device->release();
    m_textureAnimationBuffer->release();
}

void Renderer::buildMetal()
{
    generalPipeline = buildShader("general_shader.metal", "vertexMain", "fragmentMain");
    //computePipeline = buildCompute("compute.metal", "mandelbrotFn");
}

MTL::RenderPipelineState* Renderer::buildShader(const char* filename, const char* vertName, const char* fragName)
{
    NS::Error* error = nullptr;
    MTL::Library* library = _device->newDefaultLibrary();
    
    if (!library)
    {
        __builtin_printf( "%s", error->localizedDescription()->utf8String() );
        assert( false );
    }
    
    NS::String* vertexName = NS::String::string(vertName, NS::StringEncoding::UTF8StringEncoding);
    MTL::Function* vertexMain = library->newFunction(vertexName);
    
    NS::String* fragmentName = NS::String::string(fragName, NS::StringEncoding::UTF8StringEncoding);
    MTL::Function* fragmentMain = library->newFunction(fragmentName);
    
    MTL::RenderPipelineDescriptor* pipelineDescriptor = MTL::RenderPipelineDescriptor::alloc()->init();
    pipelineDescriptor->setVertexFunction(vertexMain);
    pipelineDescriptor->setFragmentFunction(fragmentMain);
    pipelineDescriptor->colorAttachments()
                        ->object(0)
                        ->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);
    pipelineDescriptor->setDepthAttachmentPixelFormat(MTL::PixelFormat::PixelFormatDepth16Unorm);

    MTL::VertexDescriptor* vertexDescriptor = MTL::VertexDescriptor::alloc()->init();
    auto attributes = vertexDescriptor->attributes();
    
    auto positionDescriptor = attributes->object(0);
    positionDescriptor->setFormat(MTL::VertexFormat::VertexFormatFloat2);
    positionDescriptor->setBufferIndex(0);
    positionDescriptor->setOffset(0);
    
    auto colorDescriptor = attributes->object(1);
    colorDescriptor->setFormat(MTL::VertexFormat::VertexFormatFloat3);
    colorDescriptor->setBufferIndex(0);
    colorDescriptor->setOffset(4* sizeof(float));
    
    MTL::RenderPipelineColorAttachmentDescriptor *colorAttDesc = pipelineDescriptor->colorAttachments()->object(0);
    colorAttDesc->setBlendingEnabled(YES);
    colorAttDesc->setRgbBlendOperation(MTL::BlendOperationAdd);
    colorAttDesc->setAlphaBlendOperation(MTL::BlendOperationAdd);
    colorAttDesc->setSourceRGBBlendFactor(MTL::BlendFactorSourceAlpha);
    colorAttDesc->setSourceAlphaBlendFactor(MTL::BlendFactorSourceAlpha);
    colorAttDesc->setDestinationRGBBlendFactor(MTL::BlendFactorOneMinusSourceAlpha);
    colorAttDesc->setDestinationAlphaBlendFactor(MTL::BlendFactorOneMinusSourceAlpha);
    
    auto layoutDescriptor = vertexDescriptor->layouts()->object(0);
    layoutDescriptor->setStride(8*sizeof(float));
    
    pipelineDescriptor->setVertexDescriptor(vertexDescriptor);
    
    MTL::RenderPipelineState* pipeline = _device->newRenderPipelineState(pipelineDescriptor, &error);
    if (!pipeline)
    {
        __builtin_printf( "%s", error->localizedDescription()->utf8String() );
        assert( false );
    }
    
    vertexMain->release();
    fragmentMain->release();
    pipelineDescriptor->release();
    library->release();
    
    return pipeline;
}

MTL::ComputePipelineState* Renderer::buildCompute(const char* filename, const char* compName)
{
    NS::Error* error = nullptr;
    
    MTL::Library* computeLibrary = _device->newDefaultLibrary();
    if (!computeLibrary){
        __builtin_printf( "%s", error->localizedDescription()->utf8String() );
        assert( false );
    }
    
    NS::String* computeName =NS::String::string(compName, NS::UTF8StringEncoding);
    MTL::Function* computeFunction = computeLibrary->newFunction(computeName);
    MTL::ComputePipelineState* pipeline = _device->newComputePipelineState(computeFunction, &error);
    
    if (!pipeline)
    {
        __builtin_printf( "%s", error->localizedDescription()->utf8String() );
        assert( false );
    }
    
    computeFunction->release();
    computeLibrary->release();
    
    return pipeline;
}

void Renderer::buildDepthStencilState()
{
    MTL::DepthStencilDescriptor* DSDesc = MTL::DepthStencilDescriptor::alloc()->init();
    DSDesc->setDepthCompareFunction(MTL::CompareFunction::CompareFunctionLess);
    DSDesc->setDepthWriteEnabled(true);
    
    m_depthStencilState = _device->newDepthStencilState(DSDesc);
    
    DSDesc->release();
}

void Renderer::generateTextures(){
    const uint32_t tw = kTextureWidth;
    const uint32_t th = kTextureHeight;
    
    MTL::TextureDescriptor* textureDesc = MTL::TextureDescriptor::alloc()->init();
    textureDesc->setWidth(tw);
    textureDesc->setHeight(th);
    textureDesc->setPixelFormat(MTL::PixelFormatRGBA8Unorm);
    textureDesc->setTextureType(MTL::TextureType2D);
    textureDesc->setStorageMode(MTL::StorageModeShared);
    textureDesc->setUsage(MTL::ResourceUsageSample|MTL::ResourceUsageRead|MTL::ResourceUsageWrite);
    
    MTL::Texture *texture = _device->newTexture(textureDesc);
    m_texture = texture;
    int lineSize = 4;
    
    uint8_t* textureData = (uint8_t*)alloca(tw*th*4);
    for (size_t y = 0; y<th; ++y){
        for (size_t x = 0; x<tw; ++x){
            bool isWhite = (x^y) & 0b1000000;
            uint8_t c = isWhite ? 0xFF : 0xA;
        
            size_t i = y*tw+x;
            if (y<lineSize||y>th-lineSize||x<lineSize||x>tw-lineSize){
                textureData[i*4 + 0] =0xFF;
                textureData[i*4 + 1] = 0xFF;
                textureData[i*4 + 2] = 0xFF;
                textureData[i*4 + 3] = 0xFF;
            }
            else{
                textureData[i*4 + 0] = c;
                textureData[i*4 + 1] = c;
                textureData[i*4 + 2] = c;
                textureData[i*4 + 3] = c;
            }
            
        }
    }
    
    m_texture->replaceRegion(MTL::Region(0, 0, 0, tw, th, 1), 0, textureData, tw*4);
    
    textureDesc->release();
}

void Renderer::buildMeshes()
{
    TheMesh = new MeshFactory(_device, kSphereSize, kSphereSize);//buildSphere(_device);//MeshFactory::buildQuad(_device);
}

void Renderer::draw(MTK::View *view)
{
    using simd::float3;
    using simd::float4;
    using simd::float4x4;
    
    NS::AutoreleasePool* pool = NS::AutoreleasePool::alloc()->init();
    _frame = (_frame + 1) % kMaxFramesInFlight;
    MTL::CommandBuffer* commandBuffer = commandQueue->commandBuffer();

    dispatch_semaphore_wait( m_semaphore, DISPATCH_TIME_FOREVER );
    Renderer* pRenderer = this;
    commandBuffer->addCompletedHandler(^void(MTL::CommandBuffer* commandBuffer){
        dispatch_semaphore_signal(pRenderer->m_semaphore);
    });
    
    quadMesh = TheMesh->buildPlane(commandBuffer, commandQueue);
    
    MTL::Buffer* currInstanceBuffer = quadMesh.instanceBuffer[_frame];
    MTL::Buffer* currCameraBuffer = quadMesh.cameraBuffer[_frame];
    
    _angle += 0.002f;
    const float scl = 0.3f;
    float3 objectPosition = { 0.f, 0.f, -10.f };
    
    //Instance----------------------------------------------------------------------------
    InstanceData* instanceData = reinterpret_cast<InstanceData*>(currInstanceBuffer->contents());

    
    float4x4 rt = trans::translation( objectPosition );
    float4x4 scale = trans::scale( (float3){ scl, scl, scl } );
    float4x4 rot = trans::y_rotation(1)*trans::x_rotation(1.2);
    
    size_t ix = 0;
    size_t iy =0;
    size_t iz =0;
    
    for ( size_t i = 0; i < kNumInstances; ++i )
    {
        if (ix == kInstanceRows){
            ix = 0;
            iy+=1;
        }
        
        if (iy== kInstanceColumns){
            iy = 0;
            iz +=1;
        }


        // Use the tiny math library to apply a 3D transformation to the instance.


        instanceData[ i ].instanceTransform =  rt * scale*rot;
        instanceData[ i ].instanceNormalTransform = trans::discardTrans(instanceData[i].instanceTransform);
        instanceData[ i ].instanceColor = (float4){ 1.f, 1.f, 1.f, 1.0f };
        
        ix +=1;
    }
    
    //Camera  ----------------------------------------------------------------------------
    CameraData* cameraData = reinterpret_cast< CameraData *>( currCameraBuffer->contents() );
    cameraData->perspectiveTransform = trans::perspective( 3.f * M_PI / 180.f, 1.f, .3f, 500.0f ) ;
    cameraData->worldTransform = trans::identity();
    cameraData->worldNormalTransform = trans::discardTrans(cameraData->worldTransform);
    
    //Texture ----------------------------------------------------------------------------
    
    //generateMandelbrot(commandBuffer);
    
    //Render  ----------------------------------------------------------------------------
    MTL::RenderPassDescriptor* renderPass = view->currentRenderPassDescriptor();
    MTL::RenderCommandEncoder* encoder = commandBuffer->renderCommandEncoder(renderPass);
    
    encoder->setRenderPipelineState(generalPipeline);
    encoder->setDepthStencilState(m_depthStencilState);
    
    encoder->setVertexBuffer(quadMesh.vertexBuffer, 0, 0);
    encoder->setVertexBuffer(currInstanceBuffer, 0, 1);
    encoder->setVertexBuffer(currCameraBuffer, 0, 2);
    
    encoder->setFragmentTexture(m_texture, 0);
    
    encoder->setCullMode(MTL::CullModeBack);
    encoder->setFrontFacingWinding(MTL::Winding::WindingCounterClockwise);
    encoder->drawIndexedPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangleStrip,
                                   (kSphereSize)*(kSphereSize)*2+kSphereSize,MTL::IndexType::IndexTypeUInt16,
                                   quadMesh.indexBuffer,
                                   0,
                                   kNumInstances);
    
    encoder->endEncoding();
    commandBuffer->presentDrawable(view->currentDrawable());
    commandBuffer->commit();
    //commandBuffer->waitUntilCompleted();
    
    pool->release();
    //unsigned int microsecond = 1000000;
    //usleep(0.1*microsecond);
}
