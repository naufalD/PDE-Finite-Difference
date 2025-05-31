//
//  view_delegate.cpp
//  metaltest
//
//  Created by Color Yeti on 1/9/24.
//

#include "view_delegate.hpp"

ViewDelegate::ViewDelegate(MTL::Device* device) 
: MTK::ViewDelegate()
, renderer(new Renderer(device))
{
    
}


ViewDelegate::~ViewDelegate()
{
    delete renderer;
}

void ViewDelegate::drawInMTKView(MTK::View* view)
{
    renderer->draw(view);
}
