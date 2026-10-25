// SPDX-License-Identifier: GPL-2.0-or-later
#define NOMINMAX
#include "backend.hpp"
#include "adapter-policy.hpp"
#include <windows.h>
#include <dxgi1_2.h>
#include <d3d11.h>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <map>
#include <cstring>
#include <stdexcept>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/buffer.h>
#include <libavutil/error.h>
#include <libavutil/frame.h>
#include <libavutil/hwcontext.h>
#include <libavutil/hwcontext_d3d11va.h>
#include <libavutil/opt.h>
}
namespace gbe {
static std::string error_text(int code) {
    char text[AV_ERROR_MAX_STRING_SIZE]={};
    av_strerror(code,text,sizeof(text));
    return text;
}
static std::string utf8(const wchar_t* text) {
    int len=WideCharToMultiByte(CP_UTF8,0,text,-1,nullptr,0,nullptr,nullptr);
    if(len<=1)return {};
    std::vector<char> buffer(len);
    WideCharToMultiByte(CP_UTF8,0,text,-1,buffer.data(),len,nullptr,nullptr);
    return buffer.data();
}
const std::vector<Codec>& codecs() {
    static const std::vector<Codec> value={
        {"h264","H.264","h264_amf"},{"hevc","H.265 / HEVC","hevc_amf"},{"av1","AV1","av1_amf"}};
    return value;
}

// Minimal Windows SDK KMT ABI declarations; no full WDK is required to build.
static AdapterType query_adapter_type(LUID luid) {
    struct OpenLuid {LUID luid;UINT handle;};
    struct Query {UINT handle;UINT type;void* data;UINT size;};
    struct Close {UINT handle;};
    static_assert(sizeof(Query)==24,"Windows x64 KMT ABI");
    using OpenFn=LONG(WINAPI*)(OpenLuid*);
    using QueryFn=LONG(WINAPI*)(const Query*);
    using CloseFn=LONG(WINAPI*)(const Close*);
    static HMODULE module=LoadLibraryExW(L"gdi32.dll",nullptr,LOAD_LIBRARY_SEARCH_SYSTEM32);
    if(!module)return {};
    static auto open=reinterpret_cast<OpenFn>(GetProcAddress(module,"D3DKMTOpenAdapterFromLuid"));
    static auto query=reinterpret_cast<QueryFn>(GetProcAddress(module,"D3DKMTQueryAdapterInfo"));
    static auto close=reinterpret_cast<CloseFn>(GetProcAddress(module,"D3DKMTCloseAdapter"));
    if(!open||!query||!close)return {};
    OpenLuid request={luid,0};
    if(open(&request)<0)return {};
    uint32_t flags=0;
    constexpr UINT KMTQAITYPE_ADAPTERTYPE_VALUE=15;
    Query info={request.handle,KMTQAITYPE_ADAPTERTYPE_VALUE,&flags,sizeof(flags)};
    LONG result=query(&info);
    Close release={request.handle};close(&release);
    return {result>=0,flags};
}

std::vector<Adapter> adapters() {
    std::vector<Adapter> result;
    IDXGIFactory1* factory=nullptr;
    if(FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory1),reinterpret_cast<void**>(&factory))))
        throw std::runtime_error("Cannot enumerate DXGI adapters");
    std::map<std::string,unsigned> duplicates;
    for(unsigned index=0;;++index) {
        IDXGIAdapter1* device=nullptr;
        if(factory->EnumAdapters1(index,&device)==DXGI_ERROR_NOT_FOUND)break;
        if(!device)continue;
        DXGI_ADAPTER_DESC1 desc={};
        HRESULT hr=device->GetDesc1(&desc);
        device->Release();
        if(FAILED(hr))continue;
        if(!encoding_candidate(!!(desc.Flags&DXGI_ADAPTER_FLAG_SOFTWARE),query_adapter_type(desc.AdapterLuid)))continue;
        Adapter a;
        a.index=index;a.vendor=desc.VendorId;a.device=desc.DeviceId;
        a.subsystem=desc.SubSysId;a.revision=desc.Revision;
        a.name=utf8(desc.Description);a.dedicated_bytes=desc.DedicatedVideoMemory;
        a.luid=(uint64_t(uint32_t(desc.AdapterLuid.HighPart))<<32)|desc.AdapterLuid.LowPart;
        std::ostringstream key;
        key<<std::hex<<a.vendor<<"_"<<a.device<<"_"<<a.subsystem<<"_"<<a.revision;
        a.ordinal=duplicates[key.str()]++;
        a.identity=key.str()+"_"+std::to_string(a.ordinal);
        result.push_back(a);
    }
    factory->Release();
    return result;
}
std::string library_versions() {
    std::ostringstream s;s<<"libavcodec "<<avcodec_version()<<" / libavutil "<<avutil_version();return s.str();
}
Encoder::Encoder() {}
Encoder::~Encoder() {
    av_packet_free(&packet_);
    av_frame_free(&frame_);
    avcodec_free_context(&context_);
    av_buffer_unref(&device_);
}
bool Encoder::open(const Adapter& adapter,const Codec& codec,const Settings& settings,std::string& error) {
    if(context_) {error="Encoder is already open";return false;}
    if((avcodec_version()>>16)!=LIBAVCODEC_VERSION_MAJOR ||
       (avutil_version()>>16)!=LIBAVUTIL_VERSION_MAJOR) {
        error="Unsupported FFmpeg ABI; rebuild against this OBS version";return false;
    }
    if(adapter.vendor!=0x1002) {error="This preview implements AMD AMF only";return false;}
    if(settings.width<64||settings.height<64||settings.width>8192||settings.height>8192||
       settings.width%2||settings.height%2||settings.fps_num<=0||settings.fps_den<=0||
       settings.bitrate<250||settings.bitrate>200000||settings.keyint<1||settings.keyint>20) {
        error="Invalid dimensions, frame rate, bitrate or keyframe interval";return false;
    }
    adapter_=adapter;settings_=settings;
    const AVCodec* impl=avcodec_find_encoder_by_name(codec.ffmpeg.c_str());
    if(!impl) {error="OBS FFmpeg does not include "+codec.ffmpeg;return false;}
    // Bind to an actual adapter, never fall back to the rendering adapter or CPU.
    IDXGIFactory1* factory=nullptr;
    IDXGIAdapter1* selected=nullptr;
    if(FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory1),reinterpret_cast<void**>(&factory)))) {
        error="CreateDXGIFactory1 failed";return false;
    }
    HRESULT hr=factory->EnumAdapters1(adapter.index,&selected);
    factory->Release();
    if(FAILED(hr)||!selected) {error="Selected adapter disappeared; restart OBS";return false;}
    DXGI_ADAPTER_DESC1 desc={};selected->GetDesc1(&desc);
    uint64_t luid=(uint64_t(uint32_t(desc.AdapterLuid.HighPart))<<32)|desc.AdapterLuid.LowPart;
    if(luid!=adapter.luid) {selected->Release();error="GPU order changed; restart OBS";return false;}
    ID3D11Device* d3d=nullptr;
    D3D_FEATURE_LEVEL level;
    hr=D3D11CreateDevice(selected,D3D_DRIVER_TYPE_UNKNOWN,nullptr,D3D11_CREATE_DEVICE_VIDEO_SUPPORT,
                         nullptr,0,D3D11_SDK_VERSION,&d3d,&level,nullptr);
    selected->Release();
    if(FAILED(hr)) {error="Cannot create D3D11 device on selected GPU";return false;}
    device_=av_hwdevice_ctx_alloc(AV_HWDEVICE_TYPE_D3D11VA);
    if(!device_) {d3d->Release();error="Cannot allocate D3D11 hardware context";return false;}
    auto hw=static_cast<AVHWDeviceContext*>(reinterpret_cast<void*>(device_->data));
    auto dx=static_cast<AVD3D11VADeviceContext*>(hw->hwctx);
    dx->device=d3d; // Ownership passes to the FFmpeg context.
    int rc=av_hwdevice_ctx_init(device_);
    if(rc<0) {error="D3D11 initialization: "+error_text(rc);return false;}
    context_=avcodec_alloc_context3(impl);
    if(!context_) {error="Cannot allocate codec context";return false;}
    context_->width=settings.width;context_->height=settings.height;
    context_->pix_fmt=AV_PIX_FMT_NV12;
    context_->time_base={settings.fps_den,settings.fps_num};
    context_->framerate={settings.fps_num,settings.fps_den};
    context_->bit_rate=int64_t(settings.bitrate)*1000;
    context_->rc_max_rate=context_->bit_rate*(settings.rate_control=="vbr_peak"?2:1);
    context_->rc_buffer_size=int(context_->bit_rate*2);
    context_->gop_size=int(int64_t(settings.keyint)*settings.fps_num/settings.fps_den);
    context_->max_b_frames=0;
    context_->flags|=AV_CODEC_FLAG_GLOBAL_HEADER;
    context_->color_range=settings.full_range?AVCOL_RANGE_JPEG:AVCOL_RANGE_MPEG;
    context_->colorspace=static_cast<AVColorSpace>(settings.colorspace);
    context_->color_primaries=static_cast<AVColorPrimaries>(settings.primaries);
    context_->color_trc=static_cast<AVColorTransferCharacteristic>(settings.transfer);
    context_->hw_device_ctx=av_buffer_ref(device_);
    if(!context_->hw_device_ctx){error="Cannot retain device";return false;}
    AVDictionary* options=nullptr;
    av_dict_set(&options,"usage","lowlatency",0);
    av_dict_set(&options,"quality",settings.quality.c_str(),0);
    av_dict_set(&options,"rc",settings.rate_control.c_str(),0);
    av_dict_set(&options,"enforce_hrd","1",0);
    if(codec.id=="h264"||codec.id=="hevc") av_dict_set(&options,"forced_idr","1",0);
    if(codec.id=="h264") {
        av_dict_set(&options,"profile","high",0);
        av_dict_set(&options,"bf","0",0);
        av_dict_set(&options,"filler_data",settings.rate_control=="cbr"?"1":"0",0);
    }
    rc=avcodec_open2(context_,impl,&options);
    av_dict_free(&options);
    if(rc<0) {error=codec.ffmpeg+": "+error_text(rc);return false;}
    if(context_->extradata&&context_->extradata_size>0)
        extra_.assign(context_->extradata,context_->extradata+context_->extradata_size);
    frame_=av_frame_alloc();packet_=av_packet_alloc();
    if(!frame_||!packet_) {error="Cannot allocate frame/packet";return false;}
    frame_->format=context_->pix_fmt;frame_->width=settings.width;frame_->height=settings.height;
    frame_->colorspace=context_->colorspace;frame_->color_range=context_->color_range;
    frame_->color_primaries=context_->color_primaries;frame_->color_trc=context_->color_trc;
    rc=av_frame_get_buffer(frame_,32);
    if(rc<0){error=error_text(rc);return false;}
    return true;
}
bool Encoder::drain(std::string& error) {
    for(;;) {
        int rc=avcodec_receive_packet(context_,packet_);
        if(rc==AVERROR(EAGAIN)||rc==AVERROR_EOF)return true;
        if(rc<0){error=error_text(rc);return false;}
        Packet p;p.pts=packet_->pts;p.dts=packet_->dts;p.key=!!(packet_->flags&AV_PKT_FLAG_KEY);
        p.bytes.assign(packet_->data,packet_->data+packet_->size);
        packets_.push_back(std::move(p));++packet_count_;
        av_packet_unref(packet_);
        if(packets_.size()>240){error="Encoder packet queue exceeded its limit";return false;}
    }
}
bool Encoder::send(const uint8_t* y,int pitch_y,const uint8_t* uv,int pitch_uv,int64_t pts,std::string& error) {
    if(!context_||!frame_||!y||!uv||pitch_y<settings_.width||pitch_uv<settings_.width) {
        error="Invalid NV12 frame";return false;
    }
    int rc=av_frame_make_writable(frame_);
    if(rc<0){error=error_text(rc);return false;}
    for(int row=0;row<settings_.height;++row)
        std::memcpy(frame_->data[0]+row*frame_->linesize[0],y+row*pitch_y,settings_.width);
    for(int row=0;row<settings_.height/2;++row)
        std::memcpy(frame_->data[1]+row*frame_->linesize[1],uv+row*pitch_uv,settings_.width);
    frame_->pts=pts;
    frame_->pict_type=(sent_frames_%context_->gop_size==0)?AV_PICTURE_TYPE_I:AV_PICTURE_TYPE_NONE;
    rc=avcodec_send_frame(context_,frame_);
    if(rc==AVERROR(EAGAIN)){
        if(!drain(error))return false;
        rc=avcodec_send_frame(context_,frame_);
    }
    if(rc<0){error=error_text(rc);return false;}
    ++sent_frames_;
    return drain(error);
}
bool Encoder::flush(std::string& error) {
    int rc=avcodec_send_frame(context_,nullptr);
    if(rc<0&&rc!=AVERROR_EOF&&rc!=AVERROR(EAGAIN)){error=error_text(rc);return false;}
    return drain(error);
}
bool Encoder::receive(Packet& out) {
    if(packets_.empty())return false;
    out=std::move(packets_.front());packets_.pop_front();return true;
}
bool probe(const Adapter& adapter,const Codec& codec,std::string& reason) {
    Settings s;s.width=1920;s.height=1080;s.fps_num=30;s.bitrate=1000;
    Encoder encoder;if(!encoder.open(adapter,codec,s,reason))return false;
    std::vector<uint8_t> y(s.width*s.height,32),uv(s.width*s.height/2,128);
    unsigned count=0;
    for(int i=0;i<8;++i){
        if(!encoder.send(y.data(),s.width,uv.data(),s.width,i,reason))return false;
        Packet p;while(encoder.receive(p))++count;
    }
    if(!encoder.flush(reason))return false;
    Packet p;while(encoder.receive(p))++count;
    if(!count||encoder.header().empty()) {reason="No encoded packets or codec header returned";return false;}
    return true;
}
} // namespace gbe
