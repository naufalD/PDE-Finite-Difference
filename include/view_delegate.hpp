//
//  view_delegate.hpp
//  metaltest
//
//  Created by Color Yeti on 1/9/24.
//

#ifndef view_delegate_hpp
#define view_delegate_hpp

#include "config.h"
#include "renderer.hpp"

#include <stdio.h>


class ViewDelegate : public MTK::ViewDelegate
{
public:
    ViewDelegate(MTL::Device* device);
    virtual ~ViewDelegate() override;
    virtual void drawInMTKView(MTK::View* view) override;
    
private:
    Renderer* renderer;
};

#endif /* view_delegate_hpp */
