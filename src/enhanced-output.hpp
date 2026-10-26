// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include <obs.h>
#include <string>
#include <vector>
namespace gbe {
struct EnhancedBinding {
    std::string adapter_identity, adapter_name, encoder_id, codec;
    unsigned adapter_index=0;
};
struct EnhancedRoute {
    obs_encoder_group_t* group=nullptr;
    EnhancedRoute()=default;
    EnhancedRoute(const EnhancedRoute&)=delete;
    EnhancedRoute& operator=(const EnhancedRoute&)=delete;
    ~EnhancedRoute();
};
// Called synchronously from STREAMING_STARTING, after negotiation and before output start.
// On failure, the output is deliberately blocked; the original encoders are never used.
bool route_enhanced_output(obs_output_t*, const std::string& adapter,
                           const std::vector<EnhancedBinding>&, EnhancedRoute&, std::string& error);
void register_enhanced_guard();
void start_enhanced_frontend(std::vector<EnhancedBinding>);
void stop_enhanced_frontend();
}
