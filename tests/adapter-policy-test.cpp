// SPDX-License-Identifier: GPL-2.0-or-later
#include "adapter-policy.hpp"
#include <cstdio>
#include <initializer_list>
int main() {
    using gbe::encoding_candidate;
    if(!encoding_candidate(false,{true,0x230b}))return 1; // Physical dGPU.
    if(!encoding_candidate(false,{true,0x2303}))return 2; // Integrated GPU.
    if(encoding_candidate(false,{true,0x342}))return 3;   // Display-only indirect alias.
    if(encoding_candidate(true,{false,0}))return 4;      // DXGI software adapter.
    if(encoding_candidate(false,{true,1u<<2}))return 5;  // Kernel software adapter.
    if(!encoding_candidate(false,{false,0}))return 6;    // Query unavailable: retain.
    unsigned same_model_count=0;
    for(auto flags:{0x230bu,0x230bu})if(encoding_candidate(false,{true,flags}))++same_model_count;
    if(same_model_count!=2)return 7; // Two real identical cards must survive.
    puts("PASS physical/iGPU retained, indirect alias excluded, unknown retained, identical real GPUs retained");
    return 0;
}
