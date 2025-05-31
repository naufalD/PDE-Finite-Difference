//
//  main.cpp
//  metaltest
//
//  Created by Color Yeti on 1/9/24.
//
#include <cassert>

#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define MTK_PRIVATE_IMPLEMENTATION

#include "config.h"
#include "app_delegate.hpp"


int main(int argc, const char * argv[])
{
    NS::AutoreleasePool* autoreleasePool = NS::AutoreleasePool::alloc()->init();
    
    AppDelegate controller;
    
    NS::Application* app = NS::Application::sharedApplication();
    app->setDelegate(&controller);
    app->run();
    
    autoreleasePool->release();
    
    return 0;
}
