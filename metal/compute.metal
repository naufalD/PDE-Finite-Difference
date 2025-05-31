//
//  compute.metal
//  Compute
//
//  Created by Color Yeti on 29/4/25.
//


#include "compute.h"

kernel void LaplaceGPU(constant int& width[[buffer(3)]],
                       constant float* initHeight [[buffer(4)]],
                       device float* updatedHeight [[buffer(5)]],
                       uint2 index [[thread_position_in_grid]])
{
//    if (initHeight[index.x+index.y*width]==0){
//        updatedHeight[index.x+index.y*width]=0.25*(updatedHeight[(index.x+1)+(index.y)*width]+updatedHeight[(index.x-1)+(index.y)*width]+updatedHeight[index.x+(index.y+1)*width]+updatedHeight[index.x+(index.y-1)*width]);
//    }
    
    int i = index.x;
    int j = index.y;
    
    if (initHeight[i+j*width]==0){
        updatedHeight[i+j*width] = 0.25*(updatedHeight[(i+1)+(j)*width]+updatedHeight[(i-1)+(j)*width]+updatedHeight[(i)+(j+1)*width]+updatedHeight[(i)+(j-1)*width]);
    }
}
