// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include "backend.hpp"
#include <cstring>
#include <stdexcept>
namespace gbe { namespace wire {
constexpr uint32_t magic=0x31454247;
constexpr uint32_t limit=128*1024*1024;
enum Kind:uint32_t {Probe=1,Init=2,Frame=3,Flush=4,Stop=5,Result=10,Failure=11};
struct Header {uint32_t magic_value=magic,kind=0,length=0,reserved=0;};
using Bytes=std::vector<uint8_t>;
template<typename T> void put(Bytes& b,const T& value) {
    auto p=reinterpret_cast<const uint8_t*>(&value);b.insert(b.end(),p,p+sizeof(T));
}
inline void text(Bytes& b,const std::string& s){
    put(b,uint32_t(s.size()));b.insert(b.end(),s.begin(),s.end());
}
struct Reader {
    const Bytes& b;size_t offset=0;
    template<typename T> T get(){
        if(offset+sizeof(T)>b.size())throw std::runtime_error("Truncated worker message");
        T value;std::memcpy(&value,b.data()+offset,sizeof(T));offset+=sizeof(T);return value;
    }
    std::string text(){
        auto n=get<uint32_t>();if(n>65536||offset+n>b.size())throw std::runtime_error("Invalid worker string");
        std::string s(reinterpret_cast<const char*>(b.data()+offset),n);offset+=n;return s;
    }
    Bytes bytes(size_t n){
        if(n>limit||offset+n>b.size())throw std::runtime_error("Invalid worker payload");
        Bytes v(b.begin()+offset,b.begin()+offset+n);offset+=n;return v;
    }
    void end(){if(offset!=b.size())throw std::runtime_error("Trailing worker message data");}
};
inline void put_adapter(Bytes& b,const Adapter& a){
    put(b,a.index);put(b,a.vendor);put(b,a.device);put(b,a.subsystem);put(b,a.revision);put(b,a.ordinal);
    put(b,a.luid);put(b,a.dedicated_bytes);text(b,a.name);text(b,a.identity);
}
inline Adapter get_adapter(Reader& r){
    Adapter a;a.index=r.get<unsigned>();a.vendor=r.get<unsigned>();a.device=r.get<unsigned>();
    a.subsystem=r.get<unsigned>();a.revision=r.get<unsigned>();a.ordinal=r.get<unsigned>();
    a.luid=r.get<uint64_t>();a.dedicated_bytes=r.get<uint64_t>();a.name=r.text();a.identity=r.text();return a;
}
inline void put_settings(Bytes& b,const Settings& s){
    put(b,s.width);put(b,s.height);put(b,s.fps_num);put(b,s.fps_den);put(b,s.bitrate);put(b,s.keyint);
    text(b,s.rate_control);text(b,s.quality);put(b,uint32_t(s.full_range));
    put(b,s.colorspace);put(b,s.primaries);put(b,s.transfer);
}
inline Settings get_settings(Reader& r){
    Settings s;s.width=r.get<int>();s.height=r.get<int>();s.fps_num=r.get<int>();s.fps_den=r.get<int>();
    s.bitrate=r.get<int>();s.keyint=r.get<int>();s.rate_control=r.text();s.quality=r.text();
    s.full_range=!!r.get<uint32_t>();s.colorspace=r.get<int>();s.primaries=r.get<int>();s.transfer=r.get<int>();return s;
}
inline void packets(Bytes& b,Encoder& encoder){
    std::vector<Packet> values;Packet p;while(encoder.receive(p))values.push_back(std::move(p));
    put(b,uint32_t(values.size()));
    for(const auto& value:values){
        put(b,value.pts);put(b,value.dts);put(b,uint32_t(value.key));put(b,uint32_t(value.bytes.size()));
        b.insert(b.end(),value.bytes.begin(),value.bytes.end());
    }
}
inline std::vector<Packet> get_packets(Reader& r){
    uint32_t n=r.get<uint32_t>();if(n>240)throw std::runtime_error("Worker packet count exceeded");
    std::vector<Packet> values;
    for(uint32_t k=0;k<n;++k){
        Packet p;p.pts=r.get<int64_t>();p.dts=r.get<int64_t>();p.key=!!r.get<uint32_t>();
        p.bytes=r.bytes(r.get<uint32_t>());values.push_back(std::move(p));
    }
    r.end();return values;
}
} } // namespace gbe::wire
