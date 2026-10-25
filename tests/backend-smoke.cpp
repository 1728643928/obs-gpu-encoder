// SPDX-License-Identifier: GPL-2.0-or-later
#include "backend.hpp"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <fstream>
#include <stdexcept>
int main(int argc,char** argv) {
    try {
        auto adapters=gbe::adapters();
        printf("LIBRARIES %s\n",gbe::library_versions().c_str());
        for(const auto& a:adapters) {
            printf("GPU %u %s PCI=%04x:%04x ID=%s LUID=%llx\n",a.index,a.name.c_str(),a.vendor,a.device,a.identity.c_str(),(unsigned long long)a.luid);
            if(a.vendor!=0x1002)continue;
            for(const auto& c:gbe::codecs()){
                std::string error;bool ok=gbe::probe(a,c,error);
                printf("CAPABILITY GPU=%u codec=%s supported=%d reason=%s\n",a.index,c.id.c_str(),ok,error.c_str());
            }
        }
        if(argc<3)return 0;
        unsigned index=unsigned(std::atoi(argv[1]));
        std::string wanted=argv[2];
        const gbe::Adapter* a=nullptr;const gbe::Codec* c=nullptr;
        for(auto& item:adapters)if(item.index==index)a=&item;
        for(auto& item:gbe::codecs())if(item.id==wanted)c=&item;
        if(!a||!c)return 2;
        gbe::Settings s;gbe::Encoder e;std::string error;
        if(!e.open(*a,*c,s,error)){fprintf(stderr,"OPEN FAILED %s\n",error.c_str());return 3;}
        std::vector<uint8_t> y(s.width*s.height,16),uv(s.width*s.height/2,128);
        std::ofstream output;
        if(argc>3){output.open(argv[3],std::ios::binary);output.write(reinterpret_cast<const char*>(e.header().data()),e.header().size());}
        auto start=std::chrono::steady_clock::now();int count=0,keys=0;int64_t last_pts=-1,last_dts=-1;
        auto drain=[&](){
            gbe::Packet p;while(e.receive(p)){
                if(p.pts<=last_pts||p.dts<=last_dts)throw std::runtime_error("Non-monotonic timestamps");
                last_pts=p.pts;last_dts=p.dts;++count;if(p.key)++keys;
                if(output)output.write(reinterpret_cast<const char*>(p.bytes.data()),p.bytes.size());
            }
        };
        for(int frame=0;frame<300;++frame){
            for(int row=0;row<s.height;++row)for(int col=0;col<s.width;++col)
                y[row*s.width+col]=uint8_t(16+((col/8+row/8+frame*2)%220));
            if(!e.send(y.data(),s.width,uv.data(),s.width,frame,error)){
                fprintf(stderr,"ENCODE FAILED %s\n",error.c_str());return 4;}
            drain();
            std::this_thread::sleep_until(start+std::chrono::microseconds(int64_t(frame+1)*1000000/60));
        }
        if(!e.flush(error)){fprintf(stderr,"FLUSH FAILED %s\n",error.c_str());return 5;}drain();
        auto ms=std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()-start).count();
        printf("RESULT GPU=%u codec=%s submitted=300 packets=%d keyframes=%d milliseconds=%lld header=%zu\n",index,c->id.c_str(),count,keys,(long long)ms,e.header().size());
        return count==300&&keys>=3?0:6;
    }catch(const std::exception& e){fprintf(stderr,"%s\n",e.what());return 10;}
}
