// SPDX-License-Identifier: GPL-2.0-or-later
#include <obs-module.h>
#include <util/platform.h>
#include "backend.hpp"
#include "worker-client.hpp"
#include "version.hpp"
#include "enhanced-output.hpp"
#define NOMINMAX
#include <windows.h>
#include <memory>
#include <string>
#include <vector>
#include <cstdio>
#include <exception>
OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("obs-gpu-encoder","en-US")
MODULE_EXPORT const char* obs_module_name(void) {return "GPU Encoder Bridge";}
MODULE_EXPORT const char* obs_module_description(void) {return "Select an AMD encoding GPU independently of the OBS rendering GPU.";}
MODULE_EXPORT const char* obs_module_author(void) {return "GPU Encoder Bridge contributors";}

namespace {
struct Definition {
    gbe::Adapter adapter;gbe::Codec codec;std::string id,name;
};
std::vector<std::unique_ptr<Definition>> definitions;
struct Instance {
    gbe::WorkerEncoder backend;
    obs_encoder_t* obs=nullptr;
    Definition* definition=nullptr;
    gbe::Packet packet;
    uint64_t started_ns=0,submitted=0;
    int fps_num=60,fps_den=1;
};
const char* display_name(void* type_data) {
    return static_cast<Definition*>(type_data)->name.c_str();
}
void defaults(obs_data_t* settings) {
    obs_data_set_default_int(settings,"bitrate",6000);
    obs_data_set_default_int(settings,"keyint_sec",2);
    obs_data_set_default_string(settings,"rate_control","cbr");
    obs_data_set_default_string(settings,"quality","balanced");
}
obs_properties_t* properties(void*,void* type_data) {
    auto d=static_cast<Definition*>(type_data);
    auto p=obs_properties_create();
    obs_properties_add_text(p,"transfer_info",obs_module_text("TransferInfo"),OBS_TEXT_INFO);
    auto rate=obs_properties_add_list(p,"rate_control",obs_module_text("RateControl"),OBS_COMBO_TYPE_LIST,OBS_COMBO_FORMAT_STRING);
    obs_property_list_add_string(rate,"CBR","cbr");
    obs_property_list_add_string(rate,obs_module_text("RateVBR"),"vbr_peak");
    obs_properties_add_int(p,"bitrate",obs_module_text("Bitrate"),250,200000,250);
    obs_properties_add_int(p,"keyint_sec",obs_module_text("KeyframeInterval"),1,20,1);
    auto quality=obs_properties_add_list(p,"quality",obs_module_text("Quality"),OBS_COMBO_TYPE_LIST,OBS_COMBO_FORMAT_STRING);
    obs_property_list_add_string(quality,obs_module_text("QualityBalanced"),"balanced");
    obs_property_list_add_string(quality,obs_module_text("QualityQuality"),"quality");
    obs_property_list_add_string(quality,obs_module_text("QualitySpeed"),"speed");
    return p;
}
void video_info(void*,video_scale_info* info) {
    info->format=VIDEO_FORMAT_NV12;
}
void* create(obs_data_t* settings,obs_encoder_t* obs) {
    try {
        auto d=static_cast<Definition*>(obs_encoder_get_type_data(obs));
        auto voi=video_output_get_info(obs_encoder_video(obs));
        if(!d||!voi)return nullptr;
        if(voi->colorspace==VIDEO_CS_2100_PQ||voi->colorspace==VIDEO_CS_2100_HLG||
           voi->format==VIDEO_FORMAT_P010||voi->format==VIDEO_FORMAT_I010) {
            obs_encoder_set_last_error(obs,obs_module_text("UnsupportedHDR"));return nullptr;
        }
        std::unique_ptr<Instance> i(new Instance);
        i->obs=obs;i->definition=d;i->fps_num=voi->fps_num;i->fps_den=voi->fps_den;
        gbe::Settings s;
        s.width=obs_encoder_get_width(obs);s.height=obs_encoder_get_height(obs);
        s.fps_num=voi->fps_num;s.fps_den=voi->fps_den;
        s.bitrate=int(obs_data_get_int(settings,"bitrate"));
        s.keyint=int(obs_data_get_int(settings,"keyint_sec"));
        s.rate_control=obs_data_get_string(settings,"rate_control");
        s.quality=obs_data_get_string(settings,"quality");
        if(s.rate_control!="cbr"&&s.rate_control!="vbr_peak")s.rate_control="cbr";
        if(s.quality!="speed"&&s.quality!="balanced"&&s.quality!="quality")s.quality="balanced";
        s.full_range=voi->range==VIDEO_RANGE_FULL;
        if(voi->colorspace==VIDEO_CS_601){s.colorspace=6;s.primaries=6;s.transfer=6;}
        if(voi->colorspace==VIDEO_CS_SRGB)s.transfer=13;
        std::string error;
        if(!i->backend.open(d->adapter,d->codec,s,error)) {
            obs_encoder_set_last_error(obs,error.c_str());
            blog(LOG_ERROR,"[gpu-encoder] %s: %s",d->name.c_str(),error.c_str());return nullptr;
        }
        blog(LOG_INFO,"[gpu-encoder] START selected=%s index=%u PCI=%04x:%04x codec=%s size=%dx%d fps=%d/%d bitrate=%d transfer=CPU-NV12->isolated-worker->selected-D3D11",
             d->adapter.name.c_str(),d->adapter.index,d->adapter.vendor,d->adapter.device,
             d->codec.id.c_str(),s.width,s.height,s.fps_num,s.fps_den,s.bitrate);
        return i.release();
    } catch(const std::exception& e) {
        obs_encoder_set_last_error(obs,e.what());return nullptr;
    } catch(...) {obs_encoder_set_last_error(obs,obs_module_text("UnknownInitializeError"));return nullptr;}
}
void destroy(void* data) {
    auto i=static_cast<Instance*>(data);
    if(!i)return;
    blog(LOG_INFO,"[gpu-encoder] STOP selected=%s submitted=%llu packets=%u",
         i->definition->adapter.name.c_str(),(unsigned long long)i->submitted,i->backend.encoded_packets());
    delete i;
}
bool encode(void* data,encoder_frame* frame,encoder_packet* packet,bool* received) {
    auto i=static_cast<Instance*>(data);*received=false;
    try {
        std::string error;
        if(frame) {
            if(!i->started_ns)i->started_ns=os_gettime_ns();
            if(!i->backend.send(frame->data[0],frame->linesize[0],frame->data[1],frame->linesize[1],frame->pts,error)) {
                obs_encoder_set_last_error(i->obs,error.c_str());
                blog(LOG_ERROR,"[gpu-encoder] Encode failed: %s",error.c_str());return false;
            }
            ++i->submitted;
        } else if(!i->backend.flush(error))return false;
        if(i->backend.receive(i->packet)) {
            packet->data=i->packet.bytes.data();packet->size=i->packet.bytes.size();
            packet->pts=i->packet.pts;packet->dts=i->packet.dts;
            packet->type=OBS_ENCODER_VIDEO;packet->keyframe=i->packet.key;
            *received=true;
            int64_t lag=frame?frame->pts-i->packet.pts:0;
            if(lag>int64_t(i->fps_num)*5/i->fps_den) {
                obs_encoder_set_last_error(i->obs,obs_module_text("EncodingDelay"));return false;
            }
        }
        return true;
    } catch(const std::exception& e) {obs_encoder_set_last_error(i->obs,e.what());return false;}
      catch(...) {obs_encoder_set_last_error(i->obs,obs_module_text("UnknownEncodingError"));return false;}
}
bool extra(void* data,uint8_t** bytes,size_t* size) {
    auto i=static_cast<Instance*>(data);auto& h=i->backend.header();
    *bytes=const_cast<uint8_t*>(h.data());*size=h.size();return !h.empty();
}
}
bool obs_module_load(void) {
    try {
        blog(LOG_INFO,"[gpu-encoder] GPU Encoder Bridge " GBE_VERSION "; %s",gbe::library_versions().c_str());
        HMODULE module=nullptr;
        GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCWSTR>(&obs_module_load),&module);
        wchar_t module_path[32768]={},exe_path[32768]={};
        GetModuleFileNameW(module,module_path,32768);GetModuleFileNameW(nullptr,exe_path,32768);
        std::wstring binary=module_path,directory=exe_path;
        binary=binary.substr(0,binary.find_last_of(L"\\/"))+L"\\obs-gpu-encoder-worker.exe";
        directory=directory.substr(0,directory.find_last_of(L"\\/"));
        gbe::set_worker_location(binary,directory);
        auto capabilities=gbe::worker_capabilities();
        for(const auto& capability:capabilities) {
            const auto& adapter=capability.adapter;
            blog(LOG_INFO,"[gpu-encoder] Adapter %u: %s PCI=%04x:%04x",adapter.index,adapter.name.c_str(),adapter.vendor,adapter.device);
            if(adapter.vendor!=0x1002) {
                blog(LOG_INFO,"[gpu-encoder] Adapter %u: backend not implemented (AMD preview)",adapter.index);continue;
            }
            for(const auto& codec:gbe::codecs()) {
                size_t codec_index=size_t(&codec-gbe::codecs().data());
                std::string reason=capability.reasons[codec_index];
                if(!(capability.mask&(1u<<codec_index))) {
                    blog(LOG_INFO,"[gpu-encoder] Not offered: GPU %u %s: %s",adapter.index,codec.id.c_str(),reason.c_str());continue;
                }
                std::unique_ptr<Definition> d(new Definition);
                d->adapter=adapter;d->codec=codec;
                d->id="gpu_bridge_amf_"+adapter.identity+"_"+codec.id;
                d->name=adapter.name+" — "+codec.label+" [GPU "+std::to_string(adapter.index)+" / AMF Bridge]";
                obs_encoder_info info={};
                info.id=d->id.c_str();info.type=OBS_ENCODER_VIDEO;info.codec=d->codec.id.c_str();
                info.get_name=display_name;info.create=create;info.destroy=destroy;info.encode=encode;
                info.get_defaults=defaults;info.get_properties2=properties;
                info.get_extra_data=extra;info.get_video_info=video_info;info.type_data=d.get();
                obs_register_encoder(&info);
                blog(LOG_INFO,"[gpu-encoder] Registered %s (%s)",d->id.c_str(),d->name.c_str());
                definitions.push_back(std::move(d));
            }
        }
        std::vector<gbe::EnhancedBinding> bindings;
        for(const auto& d:definitions)bindings.push_back({d->adapter.identity,d->adapter.name,d->id,d->codec.id,d->adapter.index});
        gbe::start_enhanced_frontend(std::move(bindings));
        return true;
    } catch(const std::exception& e) {blog(LOG_ERROR,"[gpu-encoder] Load failed: %s",e.what());return false;}
      catch(...) {blog(LOG_ERROR,"[gpu-encoder] Load failed");return false;}
}
// Definitions deliberately live until DLL unload; OBS may destroy encoders after module_unload.

void obs_module_unload(void) {gbe::stop_enhanced_frontend();}
