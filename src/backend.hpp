// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <deque>
#include <memory>

struct AVCodecContext;
struct AVFrame;
struct AVPacket;
struct AVBufferRef;
namespace gbe {
struct Adapter {
    unsigned index = 0, vendor = 0, device = 0, subsystem = 0, revision = 0, ordinal = 0;
    uint64_t luid = 0, dedicated_bytes = 0;
    std::string name, identity;
};
struct Codec {
    std::string id, label, ffmpeg;
};
const std::vector<Codec>& codecs();
std::vector<Adapter> adapters();
struct Settings {
    int width=1920,height=1080,fps_num=60,fps_den=1,bitrate=6000,keyint=2;
    std::string rate_control="cbr",quality="balanced";
    bool full_range=false;
    int colorspace=1,primaries=1,transfer=1; // FFmpeg BT.709 constants
};
struct Packet {
    std::vector<uint8_t> bytes;
    int64_t pts=0,dts=0;
    bool key=false;
};
class Encoder {
public:
    Encoder();
    ~Encoder();
    Encoder(const Encoder&)=delete;
    Encoder& operator=(const Encoder&)=delete;
    bool open(const Adapter&,const Codec&,const Settings&,std::string&);
    bool send(const uint8_t* y,int pitch_y,const uint8_t* uv,int pitch_uv,int64_t pts,std::string&);
    bool flush(std::string&);
    bool receive(Packet&);
    const std::vector<uint8_t>& header() const {return extra_;}
    const Adapter& selected_adapter() const {return adapter_;}
    size_t queued_packets() const {return packets_.size();}
    unsigned encoded_packets() const {return packet_count_;}
private:
    bool drain(std::string&);
    AVCodecContext* context_=nullptr;
    AVFrame* frame_=nullptr;
    AVPacket* packet_=nullptr;
    AVBufferRef* device_=nullptr;
    Settings settings_;
    Adapter adapter_;
    std::vector<uint8_t> extra_;
    std::deque<Packet> packets_;
    unsigned packet_count_=0;
    uint64_t sent_frames_=0;
};
bool probe(const Adapter&,const Codec&,std::string&);
std::string library_versions();
} // namespace gbe
