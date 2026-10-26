// SPDX-License-Identifier: GPL-2.0-or-later
#include "enhanced-output.hpp"
#include <obs-module.h>
#include <media-io/video-io.h>
#include <media-io/video-frame.h>
#include <util/platform.h>
#define NOMINMAX
#include <windows.h>
#include <array>
#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>
extern "C" const char* obs_module_text(const char* key){return key;}
namespace {
void require(bool ok,const char* message){if(!ok)throw std::runtime_error(message);}
std::atomic<unsigned> old_created{0},new_created{0};
struct Capture {
    obs_output_t* output;
    std::array<std::atomic<unsigned>,MAX_OUTPUT_VIDEO_ENCODERS> packets{};
    std::array<int64_t,MAX_OUTPUT_VIDEO_ENCODERS> last_pts{};
    std::atomic<bool> monotonic{true};
};
Capture* capture=nullptr;
const char* name(void*){return "enhanced routing test";}
void* old_create(obs_data_t*,obs_encoder_t*){++old_created;return new int(1);}
void* new_create(obs_data_t* s,obs_encoder_t* e){if(obs_data_get_bool(s,"fixture_fail")){obs_encoder_set_last_error(e,"Fixture backend refused initialization");return nullptr;}++new_created;return new int(1);}
void destroy_encoder(void* p){delete static_cast<int*>(p);}
bool encode(void*,encoder_frame*,encoder_packet*,bool* received){*received=false;return true;}
void* output_create(obs_data_t*,obs_output_t* output){auto p=new Capture;p->output=output;p->last_pts.fill(INT64_MIN);capture=p;return p;}
void output_destroy(void* p){if(capture==p)capture=nullptr;delete static_cast<Capture*>(p);}
bool output_start(void* p) {
    auto c=static_cast<Capture*>(p);
    if(!obs_output_initialize_encoders(c->output,OBS_OUTPUT_VIDEO))return false;
    return obs_output_begin_data_capture(c->output,OBS_OUTPUT_VIDEO);
}
void output_stop(void* p,uint64_t){obs_output_end_data_capture(static_cast<Capture*>(p)->output);}
void packet(void* p,encoder_packet* packet) {
    auto c=static_cast<Capture*>(p);
    if(!packet)return;
    auto i=packet->track_idx;
    if(i>=MAX_OUTPUT_VIDEO_ENCODERS){c->monotonic=false;return;}
    if(packet->pts<=c->last_pts[i])c->monotonic=false;
    c->last_pts[i]=packet->pts;++c->packets[i];
}
void register_types() {
    for(const char* id:{"fixture_old_h264","fixture_old_av1","fixture_bridge_h264"}) {
        obs_encoder_info e={};e.id=id;e.codec=std::strstr(id,"av1")?"av1":"h264";e.type=OBS_ENCODER_VIDEO;
        e.get_name=name;e.create=std::strstr(id,"old")?old_create:new_create;e.destroy=destroy_encoder;e.encode=encode;
        obs_register_encoder(&e);
    }
    obs_output_info o={};o.id="fixture_multitrack";o.flags=OBS_OUTPUT_ENCODED|OBS_OUTPUT_VIDEO|OBS_OUTPUT_MULTI_TRACK_VIDEO;
    o.get_name=name;o.create=output_create;o.destroy=output_destroy;o.start=output_start;o.stop=output_stop;o.encoded_packet=packet;
    obs_register_output(&o);o.id="mp4_output";obs_register_output(&o);gbe::register_enhanced_guard();
}
struct Video {
    video_t* value=nullptr;bool rendered=false;
    Video(bool use_renderer=false):rendered(use_renderer){
        if(rendered){obs_video_info o={};o.graphics_module="libobs-d3d11.dll";o.adapter=0;
            o.base_width=o.output_width=1920;o.base_height=o.output_height=1080;o.fps_num=60;o.fps_den=1;
            o.output_format=VIDEO_FORMAT_NV12;o.colorspace=VIDEO_CS_709;o.range=VIDEO_RANGE_PARTIAL;
            o.gpu_conversion=true;o.scale_type=OBS_SCALE_BICUBIC;
            require(obs_reset_video(&o)==OBS_VIDEO_SUCCESS,"OBS renderer initialization");value=obs_get_video();return;}
        video_output_info v={};v.name="Enhanced routing fixture";v.format=VIDEO_FORMAT_NV12;
        v.fps_num=60;v.fps_den=1;v.width=1920;v.height=1080;v.cache_size=6;v.colorspace=VIDEO_CS_709;v.range=VIDEO_RANGE_PARTIAL;
        require(video_output_open(&value,&v)==VIDEO_OUTPUT_SUCCESS,"video output open");}
    ~Video(){if(value&&!rendered)video_output_close(value);}
};
struct Fixture {
    Video& video;
    obs_output_t* live=nullptr,*recording=nullptr,*dump=nullptr;
    std::vector<obs_encoder_t*> originals;
    Fixture(Video& v,bool av1=false):video(v) {
        live=obs_output_create("fixture_multitrack","enhanced live fixture",nullptr,nullptr);
        recording=obs_output_create("fixture_multitrack","independent recording fixture",nullptr,nullptr);
        dump=obs_output_create("mp4_output","mp4 multitrack video",nullptr,nullptr);
        require(live&&recording&&dump,"fixture outputs");
        for(int i=0;i<3;++i) {
            auto settings=obs_data_create();
            obs_data_set_int(settings,"bitrate",i==0?6000:i==1?2500:160);
            obs_data_set_int(settings,"keyint_sec",2);
            obs_data_set_string(settings,"rate_control","CBR");
            auto enc=obs_video_encoder_create(av1&&i==2?"fixture_old_av1":"fixture_old_h264",
                ("original "+std::to_string(i)).c_str(),settings,nullptr);
            obs_data_release(settings);require(enc,"fixture encoder");
            obs_encoder_set_video(enc,video.value);
            obs_encoder_set_scaled_size(enc,i==0?1920:i==1?1280:640,i==0?1080:i==1?720:360);
            obs_encoder_set_gpu_scale_type(enc,OBS_SCALE_DISABLE);
            obs_encoder_set_preferred_video_format(enc,VIDEO_FORMAT_NV12);
            obs_encoder_set_preferred_color_space(enc,VIDEO_CS_709);
            obs_encoder_set_preferred_range(enc,VIDEO_RANGE_PARTIAL);
            require(obs_encoder_set_frame_rate_divisor(enc,i==0?1:i==1?2:3),"fixture frame divisor");
            obs_output_set_video_encoder2(live,enc,size_t(i));obs_output_set_video_encoder2(dump,enc,size_t(i));originals.push_back(enc);
        }
        // Even a recording sharing an old encoder must not be silently rebound.
        obs_output_set_video_encoder2(recording,originals[0],0);
    }
    ~Fixture(){
        obs_output_release(live);obs_output_release(recording);obs_output_release(dump);
        for(auto e:originals)obs_encoder_release(e);
    }
};
std::vector<gbe::EnhancedBinding> fake_bindings{{"igpu","Integrated test GPU","fixture_bridge_h264","h264",1}};
void unit_tests(Video& v) {
    {
        Fixture f(v);gbe::EnhancedRoute state;std::string error;
        require(gbe::route_enhanced_output(f.live,"igpu",fake_bindings,state,error),"route complete h264 plan");
        for(size_t i=0;i<3;++i) {
            auto e=obs_output_get_video_encoder2(f.live,i);
            require(e!=f.originals[i],"live encoder replaced");
            require(obs_output_get_video_encoder2(f.dump,i)==e,"companion stream dump shares replacement");
            require(std::strcmp(obs_encoder_get_id(e),"fixture_bridge_h264")==0,"bound target encoder");
            require(obs_encoder_get_width(e)==obs_encoder_get_width(f.originals[i]),"width preserved");
            require(obs_encoder_get_height(e)==obs_encoder_get_height(f.originals[i]),"height preserved");
            require(obs_encoder_get_frame_rate_divisor(e)==i+1,"frame divisor preserved");
            auto info=video_output_get_info(obs_encoder_video(e));
            require(info->fps_num/info->fps_den==60/(i+1),"frame divisor not applied twice");
            auto s=obs_encoder_get_settings(e);
            require(obs_data_get_int(s,"bitrate")==int64_t(i==0?6000:i==1?2500:160),"per-track bitrate preserved");
            require(std::strcmp(obs_data_get_string(s,"rate_control"),"cbr")==0,"rate control translated");
            obs_data_release(s);

        }
        require(obs_output_initialize_encoders(f.live,OBS_OUTPUT_VIDEO),"replacement encoders initialized");
        require(obs_output_get_video_encoder2(f.recording,0)==f.originals[0],"independent recording preserved");
        require(old_created==0&&new_created==3,"original GPU encoder was never initialized");
        std::puts("PASS three tracks replaced; 60/30/20 fps, sizes and low bitrate preserved; recording unchanged");
    }
    for(int scenario=0;scenario<5;++scenario) {
        Fixture f(v,scenario==0);gbe::EnhancedRoute state;std::string error;
        if(scenario==2)obs_encoder_set_preferred_video_format(f.originals[2],VIDEO_FORMAT_P010);
        if(scenario==3){auto s=obs_encoder_get_settings(f.originals[2]);obs_data_set_int(s,"bitrate",0);obs_data_release(s);}
        if(scenario==4){auto s=obs_encoder_get_settings(f.originals[1]);obs_data_set_bool(s,"fixture_fail",true);obs_data_release(s);}
        require(!gbe::route_enhanced_output(f.live,scenario==1?"missing":"igpu",fake_bindings,state,error),"invalid plan rejected");
        require(!error.empty(),"explicit error reported");
        auto e=obs_output_get_video_encoder2(f.live,0);require(e,"failure guard attached");
        require(std::strcmp(obs_encoder_get_id(e),"gpu_bridge_enhanced_start_guard")==0,"live plan blocked");
        require(!obs_output_initialize_encoders(f.live,OBS_OUTPUT_VIDEO),"blocked plan cannot initialize");
        require(obs_output_get_video_encoder2(f.recording,0)==f.originals[0],"recording retained on failure");
        require(old_created==0,"no original GPU fallback");
    }
    std::puts("PASS unsupported AV1, missing GPU, 10-bit, malformed bitrate and backend initialization failure block streaming without fallback");
}
void hardware_test(Video& video,const char* dll,const char* locale,const char* prefix) {
    obs_module_t* module=nullptr;
    require(obs_open_module(&module,dll,locale)==MODULE_SUCCESS&&obs_init_module(module),"load actual plugin");
    std::vector<gbe::EnhancedBinding> bindings;
    const char* id=nullptr;
    for(size_t i=0;obs_enum_encoder_types(i,&id);++i)if(std::strncmp(id,prefix,std::strlen(prefix))==0)
        bindings.push_back({"hardware","Selected physical GPU",id,obs_get_encoder_codec(id),1});
    require(!bindings.empty(),"selected hardware encoder exists");
    Fixture f(video);gbe::EnhancedRoute state;std::string error;
    require(gbe::route_enhanced_output(f.live,"hardware",bindings,state,error),"bind actual hardware encoders");
    auto c=static_cast<Capture*>(obs_obj_get_data(f.live));
    require(c,"hardware capture context");
    require(obs_output_start(f.live),"start actual multitrack hardware encoding");
    // OBS's renderer supplies synchronized frames and assigns encoder-group start timestamps.
    std::this_thread::sleep_for(std::chrono::seconds(5));
    obs_output_stop(f.live);
    for(int i=0;i<100&&obs_output_active(f.live);++i)std::this_thread::sleep_for(std::chrono::milliseconds(10));
    require(!obs_output_active(f.live),"hardware output stopped");
    require(c->monotonic,"hardware timestamps strictly increasing");
    for(size_t i=0;i<3;++i){auto n=c->packets[i].load();
        std::printf("HARDWARE track=%zu packets=%u expected=%zu encoder=%s\n",i,n,300/(i+1),obs_encoder_get_id(obs_output_get_video_encoder2(f.live,i)));
        require(n>=300/(i+1)-15&&n<=300/(i+1)+15,"hardware packet count");}
    require(obs_output_get_video_encoder2(f.recording,0)==f.originals[0],"hardware run preserved recording");
    require(old_created==0,"hardware run never initialized original encoder");
    std::puts("PASS simultaneous selected-GPU multitrack encoding, timestamp ordering and recording isolation");
}
}
int main(int argc,char** argv) {
    try {
        wchar_t runtime[32768]={};
        GetModuleFileNameW(GetModuleHandleW(L"obs.dll"),runtime,32768);
        std::wstring runtime_dir=runtime;runtime_dir=runtime_dir.substr(0,runtime_dir.find_last_of(L"\\/"));
        AddDllDirectory(runtime_dir.c_str());
        SetDllDirectoryW(runtime_dir.c_str());
        require(obs_startup("en-US",nullptr,nullptr),"OBS startup");
        register_types();
        {
            Video video;unit_tests(video);

        }
        if(argc>1){require(argc==5&&std::strcmp(argv[1],"--hardware")==0,"usage: --hardware DLL locale encoder-prefix");
            Video video(true);hardware_test(video,argv[2],argv[3],argv[4]);}
        obs_shutdown();std::puts("PASS enhanced output regression suite");return 0;
    }catch(const std::exception& e){std::fprintf(stderr,"FAIL %s\n",e.what());return 1;}
}
