// SPDX-License-Identifier: GPL-2.0-or-later
#include "backend.hpp"
#include "worker-client.hpp"
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <fstream>
#include <filesystem>
#include <stdexcept>
int wmain(int argc,wchar_t** argv){
 if(argc<3)return 2;
 try{
  gbe::set_worker_location(argv[1],argv[2]);
  auto caps=gbe::worker_capabilities();
  for(auto& c:caps)printf("WORKER GPU=%u name=%s mask=%u\n",c.adapter.index,c.adapter.name.c_str(),c.mask);
  unsigned selected=argc>3?unsigned(_wtoi(argv[3])):1;
  const gbe::Adapter* a=nullptr;for(auto& c:caps)if(c.adapter.index==selected)a=&c.adapter;
  if(!a)return 3;
  gbe::Settings s;gbe::WorkerEncoder e;std::string error;
  if(!e.open(*a,gbe::codecs()[0],s,error)){fprintf(stderr,"OPEN: %s\n",error.c_str());return 4;}
  std::ofstream file;if(argc>4){file.open(std::filesystem::path(argv[4]),std::ios::binary);file.write(reinterpret_cast<const char*>(e.header().data()),e.header().size());}
  std::vector<uint8_t> y(s.width*s.height,32),uv(s.width*s.height/2,128);
  int count=0,keys=0;int64_t last=-1;
  auto drain=[&](){gbe::Packet p;while(e.receive(p)){if(p.pts<=last)throw std::runtime_error("Nonmonotonic PTS");last=p.pts;++count;if(p.key)++keys;if(file)file.write(reinterpret_cast<const char*>(p.bytes.data()),p.bytes.size());}};
  auto begin=std::chrono::steady_clock::now();
  for(int f=0;f<300;++f){
   for(int row=0;row<s.height;++row)for(int col=0;col<s.width;++col)y[size_t(row)*s.width+col]=uint8_t(16+(row/8+col/8+f*2)%220);
   if(!e.send(y.data(),s.width,uv.data(),s.width,f,error)){fprintf(stderr,"ENCODE: %s\n",error.c_str());return 5;}drain();
   std::this_thread::sleep_until(begin+std::chrono::microseconds(int64_t(f+1)*1000000/60));
  }
  if(!e.flush(error)){fprintf(stderr,"FLUSH: %s\n",error.c_str());return 6;}drain();
  printf("RESULT submitted=300 packets=%d keyframes=%d header=%zu\n",count,keys,e.header().size());
  gbe::WorkerEncoder invalid;auto wrong=*a;wrong.luid^=1;
  bool rejected=!invalid.open(wrong,gbe::codecs()[0],s,error);printf("GPU_IDENTITY_MISMATCH_REJECTED=%d\n",rejected);
  return count==300&&keys>=3&&rejected?0:7;
 }catch(const std::exception& e){fprintf(stderr,"ERROR: %s\n",e.what());return 10;}
}
