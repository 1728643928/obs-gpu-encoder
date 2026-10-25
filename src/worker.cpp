// SPDX-License-Identifier: GPL-2.0-or-later
#define NOMINMAX
#include <windows.h>
#include <string>
#include <stdexcept>
#include "backend.hpp"
#include "protocol.hpp"
extern "C" {
#include <libavutil/log.h>
}
namespace {
bool io(HANDLE h,void* data,size_t length,bool write){
    auto p=static_cast<uint8_t*>(data);
    while(length){DWORD done=0,part=DWORD(length>1024*1024?1024*1024:length);
        BOOL ok=write?WriteFile(h,p,part,&done,nullptr):ReadFile(h,p,part,&done,nullptr);
        if(!ok||!done)return false;p+=done;length-=done;
    }
    return true;
}
bool receive(gbe::wire::Header& h,gbe::wire::Bytes& b){
    if(!io(GetStdHandle(STD_INPUT_HANDLE),&h,sizeof(h),false))return false;
    if(h.magic_value!=gbe::wire::magic||h.length>gbe::wire::limit)throw std::runtime_error("Invalid request");
    b.resize(h.length);return io(GetStdHandle(STD_INPUT_HANDLE),b.data(),b.size(),false);
}
void respond(uint32_t kind,const gbe::wire::Bytes& b){
    gbe::wire::Header h;h.kind=kind;h.length=uint32_t(b.size());
    if(!io(GetStdHandle(STD_OUTPUT_HANDLE),&h,sizeof(h),true)||
       !io(GetStdHandle(STD_OUTPUT_HANDLE),const_cast<uint8_t*>(b.data()),b.size(),true))
        throw std::runtime_error("OBS disconnected");
}
}
int main(){
    av_log_set_level(AV_LOG_ERROR);
    SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);
    try {
        gbe::wire::Header h;gbe::wire::Bytes b;
        if(!receive(h,b))return 0;
        if(h.kind==gbe::wire::Probe){
            gbe::wire::Bytes result;auto adapters=gbe::adapters();
            if(adapters.size()>16)throw std::runtime_error("Too many adapters");
            gbe::wire::put(result,uint32_t(adapters.size()));
            for(const auto& a:adapters){
                gbe::wire::put_adapter(result,a);uint32_t mask=0;std::vector<std::string> reasons;
                for(size_t c=0;c<gbe::codecs().size();++c){
                    std::string reason;
                    if(a.vendor==0x1002&&gbe::probe(a,gbe::codecs()[c],reason))mask|=1u<<c;
                    else if(reason.empty())reason="Backend not implemented";
                    reasons.push_back(reason);
                }
                gbe::wire::put(result,mask);for(const auto& reason:reasons)gbe::wire::text(result,reason);
            }
            respond(gbe::wire::Result,result);return 0;
        }
        if(h.kind!=gbe::wire::Init)throw std::runtime_error("Initialization required");
        gbe::wire::Reader r{b};auto requested=gbe::wire::get_adapter(r);
        auto codec_id=r.text();auto settings=gbe::wire::get_settings(r);r.end();
        auto adapters=gbe::adapters();const gbe::Adapter* selected=nullptr;
        for(auto& a:adapters)if(a.identity==requested.identity&&a.luid==requested.luid&&a.index==requested.index)selected=&a;
        if(!selected)throw std::runtime_error("GPU changed; restart OBS");
        const gbe::Codec* codec=nullptr;for(auto& c:gbe::codecs())if(c.id==codec_id)codec=&c;
        if(!codec)throw std::runtime_error("Unknown codec");
        gbe::Encoder encoder;std::string error;
        if(!encoder.open(*selected,*codec,settings,error))throw std::runtime_error(error);
        gbe::wire::Bytes init;gbe::wire::put(init,uint32_t(encoder.header().size()));
        init.insert(init.end(),encoder.header().begin(),encoder.header().end());
        respond(gbe::wire::Result,init);
        while(receive(h,b)){
            gbe::wire::Bytes result;
            if(h.kind==gbe::wire::Frame){
                gbe::wire::Reader fr{b};int64_t pts=fr.get<int64_t>();
                size_t pixels=size_t(settings.width)*settings.height;
                if(b.size()!=sizeof(int64_t)+pixels*3/2)throw std::runtime_error("Invalid NV12 frame size");
                auto y=b.data()+sizeof(int64_t);auto uv=y+pixels;
                if(!encoder.send(y,settings.width,uv,settings.width,pts,error))throw std::runtime_error(error);
                gbe::wire::packets(result,encoder);
            }else if(h.kind==gbe::wire::Flush){
                if(!encoder.flush(error))throw std::runtime_error(error);
                gbe::wire::packets(result,encoder);
            }else if(h.kind==gbe::wire::Stop){respond(gbe::wire::Result,result);return 0;}
            else throw std::runtime_error("Unsupported request");
            respond(gbe::wire::Result,result);
        }
        return 0;
    }catch(const std::exception& e){
        try{gbe::wire::Bytes b;gbe::wire::text(b,e.what());respond(gbe::wire::Failure,b);}catch(...){}
        return 1;
    }catch(...){return 2;}
}
