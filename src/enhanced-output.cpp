// SPDX-License-Identifier: GPL-2.0-or-later
#include "enhanced-output.hpp"
#include <obs-module.h>
#include <algorithm>
#include <cstring>
#include <memory>
#include <stdexcept>
namespace gbe {
namespace {
constexpr const char* guard_id="gpu_bridge_enhanced_start_guard";
const char* guard_name(void*) {return "GPU Encoder Bridge start guard";}
void* guard_create(obs_data_t* settings,obs_encoder_t* encoder) {
    obs_encoder_set_last_error(encoder,obs_data_get_string(settings,"failure"));
    return nullptr;
}
void guard_destroy(void*) {}
bool guard_encode(void*,encoder_frame*,encoder_packet*,bool*) {return false;}
struct EncoderRelease {void operator()(obs_encoder_t* p)const {if(p)obs_encoder_release(p);}};
struct OutputRelease {void operator()(obs_output_t* p)const {if(p)obs_output_release(p);}};
using Output=std::unique_ptr<obs_output_t,OutputRelease>;
struct DataRelease {void operator()(obs_data_t* p)const {if(p)obs_data_release(p);}};
using Encoder=std::unique_ptr<obs_encoder_t,EncoderRelease>;
using Data=std::unique_ptr<obs_data_t,DataRelease>;
struct Replacement {size_t index;Encoder original,replacement;};
bool equal(const char* a,const char* b){return a&&b&&std::strcmp(a,b)==0;}
void block_output(obs_output_t* output,const std::string& reason) {
    if(!output||obs_output_active(output))return;
    Data settings(obs_data_create());
    obs_data_set_string(settings.get(),"failure",reason.c_str());
    Encoder guard(obs_video_encoder_create(guard_id,"GPU Encoder Bridge blocked enhanced start",settings.get(),nullptr));
    if(guard) {
        auto first=obs_output_get_video_encoder(output);
        obs_encoder_set_video(guard.get(),first?obs_encoder_parent_video(first):nullptr);
        for(size_t i=0;i<MAX_OUTPUT_VIDEO_ENCODERS;++i)obs_output_set_video_encoder2(output,nullptr,i);
        obs_output_set_video_encoder2(output,guard.get(),0);
    } else {
        for(size_t i=0;i<MAX_OUTPUT_VIDEO_ENCODERS;++i)obs_output_set_video_encoder2(output,nullptr,i);
    }
    obs_output_set_last_error(output,reason.c_str());
}
bool companion_output(void* context,obs_output_t* output) {
    const char* n=obs_output_get_name(output);const char* id=obs_output_get_id(output);
    if((equal(n,"mp4 multitrack video")&&equal(id,"mp4_output"))||
       (equal(n,"flv multitrack video")&&equal(id,"flv_output")))
        static_cast<std::vector<Output>*>(context)->emplace_back(obs_output_get_ref(output));
    return true;
}
Data translated_settings(obs_encoder_t* original) {
    Data input(obs_encoder_get_settings(original)),out(obs_data_create());
    obs_data_apply(out.get(),input.get());
    const char* rc=obs_data_get_string(input.get(),"rate_control");
    if(equal(rc,"CBR")||equal(rc,"cbr"))obs_data_set_string(out.get(),"rate_control","cbr");
    else if(equal(rc,"VBR")||equal(rc,"vbr_peak"))obs_data_set_string(out.get(),"rate_control","vbr_peak");
    else if(rc&&*rc)throw std::runtime_error("Unsupported enhanced-broadcast rate control");
    else obs_data_set_string(out.get(),"rate_control","cbr");
    const char* q=obs_data_get_string(input.get(),"quality");
    if(!equal(q,"speed")&&!equal(q,"balanced")&&!equal(q,"quality"))
        obs_data_set_string(out.get(),"quality","balanced");
    // Enhanced plans can include low-bitrate lower-resolution renditions.
    auto bitrate=obs_data_get_int(input.get(),"bitrate");
    if(bitrate<1||bitrate>200000)throw std::runtime_error("Invalid enhanced-broadcast bitrate");
    auto keyint=obs_data_get_int(input.get(),"keyint_sec");
    if(!keyint)obs_data_set_int(out.get(),"keyint_sec",2);
    else if(keyint<1||keyint>20)throw std::runtime_error("Invalid enhanced-broadcast keyframe interval");
    obs_data_set_bool(out.get(),"gpu_bridge_enhanced",true);
    return out;
}
}
EnhancedRoute::~EnhancedRoute(){if(group)obs_encoder_group_destroy(group);}
void register_enhanced_guard() {
    if(obs_get_encoder_codec(guard_id))return;
    obs_encoder_info info={};
    info.id=guard_id;info.type=OBS_ENCODER_VIDEO;info.codec="h264";
    info.get_name=guard_name;info.create=guard_create;info.destroy=guard_destroy;info.encode=guard_encode;
    info.caps=OBS_ENCODER_CAP_INTERNAL;
    obs_register_encoder(&info);
}
bool route_enhanced_output(obs_output_t* output,const std::string& adapter,
                           const std::vector<EnhancedBinding>& bindings,EnhancedRoute& state,std::string& error) {
    try {
        if(!output||obs_output_active(output))throw std::runtime_error("Enhanced output is unavailable or already active");
        if(adapter.empty())throw std::runtime_error("No enhanced-broadcast encoding GPU selected");
        std::vector<Replacement> tracks;
        for(size_t i=0;i<MAX_OUTPUT_VIDEO_ENCODERS;++i) {
            auto old=obs_output_get_video_encoder2(output,i);
            if(!old)continue;
            if(obs_encoder_active(old))throw std::runtime_error("Enhanced encoder is already active");
            const char* codec=obs_get_encoder_codec(obs_encoder_get_id(old));
            auto binding=std::find_if(bindings.begin(),bindings.end(),[&](const EnhancedBinding& b){
                return b.adapter_identity==adapter&&equal(codec,b.codec.c_str());
            });
            if(binding==bindings.end())throw std::runtime_error("Selected GPU is unavailable or does not support every codec in the enhanced plan");
            auto format=obs_encoder_get_preferred_video_format(old);
            auto color=obs_encoder_get_preferred_color_space(old);
            if(format==VIDEO_FORMAT_P010||format==VIDEO_FORMAT_I010||format==VIDEO_FORMAT_P216||format==VIDEO_FORMAT_P416||
               color==VIDEO_CS_2100_PQ||color==VIDEO_CS_2100_HLG)
                throw std::runtime_error("Enhanced plan requires unsupported HDR or 10-bit video");
            auto settings=translated_settings(old);
            std::string name="GPU Encoder Bridge enhanced track "+std::to_string(i);
            Encoder replacement(obs_video_encoder_create(binding->encoder_id.c_str(),name.c_str(),settings.get(),nullptr));
            if(!replacement)throw std::runtime_error("Could not create a selected-GPU enhanced encoder");
            obs_encoder_set_video(replacement.get(),obs_encoder_parent_video(old));
            obs_encoder_set_scaled_size(replacement.get(),obs_encoder_get_width(old),obs_encoder_get_height(old));
            obs_encoder_set_gpu_scale_type(replacement.get(),obs_encoder_get_scale_type(old));
            obs_encoder_set_preferred_video_format(replacement.get(),VIDEO_FORMAT_NV12);
            obs_encoder_set_preferred_color_space(replacement.get(),color);
            obs_encoder_set_preferred_range(replacement.get(),obs_encoder_get_preferred_range(old));
            if(!obs_encoder_set_frame_rate_divisor(replacement.get(),obs_encoder_get_frame_rate_divisor(old)))
                throw std::runtime_error("Could not preserve enhanced-track frame rate");
            tracks.push_back({i,Encoder(obs_encoder_get_ref(old)),std::move(replacement)});
        }
        if(tracks.empty())throw std::runtime_error("Enhanced plan has no video encoders");
        std::vector<Output> companions;obs_enum_outputs(companion_output,&companions);
        for(auto& o:companions)if(obs_output_active(o.get()))
            throw std::runtime_error("Enhanced stream-dump output is already active");
        if(state.group)throw std::runtime_error("Previous enhanced encoder group has not been released");
        state.group=obs_encoder_group_create();
        if(!state.group)throw std::runtime_error("Could not allocate enhanced encoder group");
        for(auto& t:tracks)if(!obs_encoder_set_group(t.replacement.get(),state.group))
            throw std::runtime_error("Could not synchronize enhanced encoders");
        // No output attachment changes until the complete plan has been validated.
        for(auto& t:tracks)obs_output_set_video_encoder2(output,t.replacement.get(),t.index);
        for(auto& t:tracks)if(obs_output_get_video_encoder2(output,t.index)!=t.replacement.get())
            throw std::runtime_error("OBS refused the selected-GPU enhanced encoder");
        if(!obs_output_initialize_encoders(output,OBS_OUTPUT_VIDEO)) {
            std::string details="Selected GPU could not initialize every enhanced video track";
            for(auto& t:tracks){auto reason=obs_encoder_get_last_error(t.replacement.get());if(reason&&*reason){details+=": ";details+=reason;break;}}
            throw std::runtime_error(details);
        }
        // OBS's optional multitrack stream dump shares the live encoders. Keep that
        // relationship; never touch independently configured recording outputs.
        for(auto& o:companions)for(auto& t:tracks)
            if(obs_output_get_video_encoder2(o.get(),t.index)==t.original.get())
                obs_output_set_video_encoder2(o.get(),t.replacement.get(),t.index);
        blog(LOG_INFO,"[gpu-encoder] ENHANCED bound %zu video tracks to GPU identity=%s",tracks.size(),adapter.c_str());
        return true;
    }catch(const std::exception& e) {
        error=std::string(obs_module_text("EnhancedFailed"))+" "+e.what();
    }catch(...) {error=obs_module_text("EnhancedFailed");}
    if(state.group){obs_encoder_group_destroy(state.group);state.group=nullptr;}
    block_output(output,error);
    blog(LOG_ERROR,"[gpu-encoder] ENHANCED blocked: %s",error.c_str());
    return false;
}
}
