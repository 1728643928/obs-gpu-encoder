// SPDX-License-Identifier: GPL-2.0-or-later
#define NOMINMAX
#include <windows.h>
#include <atomic>
#include <cstring>
#include <stdexcept>
#include "worker-client.hpp"
#include "protocol.hpp"
namespace gbe {
static std::wstring worker_exe,worker_dir;
static std::atomic<unsigned> serial{0};
void set_worker_location(const std::wstring& executable,const std::wstring& directory){
    worker_exe=executable;worker_dir=directory;
}
namespace {
struct Channel {
    HANDLE pipe=INVALID_HANDLE_VALUE,process=nullptr,job=nullptr;
    Channel() {
        if(worker_exe.empty())throw std::runtime_error("Worker location is unavailable");
        auto pipe_name=L"\\\\.\\pipe\\OBS-GPU-Bridge-"+std::to_wstring(GetCurrentProcessId())+L"-"+
                       std::to_wstring(++serial)+L"-"+std::to_wstring(GetTickCount64());
        pipe=CreateNamedPipeW(pipe_name.c_str(),PIPE_ACCESS_DUPLEX|FILE_FLAG_OVERLAPPED,
                PIPE_TYPE_BYTE|PIPE_READMODE_BYTE|PIPE_WAIT|PIPE_REJECT_REMOTE_CLIENTS,1,1048576,1048576,0,nullptr);
        if(pipe==INVALID_HANDLE_VALUE)throw std::runtime_error("Cannot create worker pipe");
        SECURITY_ATTRIBUTES sa={sizeof(sa),nullptr,TRUE};
        HANDLE child=CreateFileW(pipe_name.c_str(),GENERIC_READ|GENERIC_WRITE,0,&sa,OPEN_EXISTING,0,nullptr);
        if(child==INVALID_HANDLE_VALUE){CloseHandle(pipe);pipe=INVALID_HANDLE_VALUE;throw std::runtime_error("Cannot connect worker pipe");}
        OVERLAPPED connect={};connect.hEvent=CreateEventW(nullptr,TRUE,FALSE,nullptr);
        BOOL connected=ConnectNamedPipe(pipe,&connect);
        DWORD code=connected?ERROR_SUCCESS:GetLastError();
        if(!connected&&code!=ERROR_PIPE_CONNECTED&&code!=ERROR_IO_PENDING){
            CloseHandle(connect.hEvent);CloseHandle(child);CloseHandle(pipe);pipe=INVALID_HANDLE_VALUE;
            throw std::runtime_error("Cannot accept worker pipe");
        }
        if(code==ERROR_IO_PENDING)WaitForSingleObject(connect.hEvent,5000);
        CloseHandle(connect.hEvent);
        HANDLE nul=CreateFileW(L"NUL",GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,&sa,OPEN_EXISTING,0,nullptr);
        SIZE_T attribute_size=0;InitializeProcThreadAttributeList(nullptr,1,0,&attribute_size);
        std::vector<uint8_t> attribute_buffer(attribute_size);
        auto attributes=reinterpret_cast<LPPROC_THREAD_ATTRIBUTE_LIST>(attribute_buffer.data());
        bool initialized=!!InitializeProcThreadAttributeList(attributes,1,0,&attribute_size);
        HANDLE inherited[]={child,nul};
        bool updated=initialized&&!!UpdateProcThreadAttribute(attributes,0,PROC_THREAD_ATTRIBUTE_HANDLE_LIST,
                                         inherited,sizeof(inherited),nullptr,nullptr);
        STARTUPINFOEXW startup={};startup.StartupInfo.cb=sizeof(startup);
        startup.StartupInfo.dwFlags=STARTF_USESTDHANDLES;
        startup.StartupInfo.hStdInput=child;startup.StartupInfo.hStdOutput=child;startup.StartupInfo.hStdError=nul;
        startup.lpAttributeList=attributes;
        PROCESS_INFORMATION pi={};
        std::wstring command=L"\""+worker_exe+L"\"";
        BOOL ok=updated&&CreateProcessW(worker_exe.c_str(),&command[0],nullptr,nullptr,TRUE,
                         CREATE_NO_WINDOW|EXTENDED_STARTUPINFO_PRESENT,nullptr,
                         worker_dir.empty()?nullptr:worker_dir.c_str(),&startup.StartupInfo,&pi);
        if(initialized)DeleteProcThreadAttributeList(attributes);
        CloseHandle(child);if(nul!=INVALID_HANDLE_VALUE)CloseHandle(nul);
        if(!ok){CloseHandle(pipe);pipe=INVALID_HANDLE_VALUE;throw std::runtime_error("Cannot start encoding worker (check installation and DLL dependencies)");}
        process=pi.hProcess;CloseHandle(pi.hThread);
        job=CreateJobObjectW(nullptr,nullptr);
        if(job){
            JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits={};
            limits.BasicLimitInformation.LimitFlags=JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
            if(!SetInformationJobObject(job,JobObjectExtendedLimitInformation,&limits,sizeof(limits))||
               !AssignProcessToJobObject(job,process)){CloseHandle(job);job=nullptr;}
        }
    }
    ~Channel(){
        if(pipe!=INVALID_HANDLE_VALUE){CancelIoEx(pipe,nullptr);CloseHandle(pipe);}
        if(process){
            if(WaitForSingleObject(process,1000)==WAIT_TIMEOUT){TerminateProcess(process,1);WaitForSingleObject(process,2000);}
            CloseHandle(process);
        }
        if(job)CloseHandle(job);
    }
    void io(void* data,size_t size,bool writing,DWORD timeout){
        auto p=static_cast<uint8_t*>(data);ULONGLONG deadline=GetTickCount64()+timeout;
        while(size){
            OVERLAPPED ov={};ov.hEvent=CreateEventW(nullptr,TRUE,FALSE,nullptr);
            DWORD done=0,chunk=DWORD(size>1048576?1048576:size);
            BOOL ok=writing?WriteFile(pipe,p,chunk,&done,&ov):ReadFile(pipe,p,chunk,&done,&ov);
            DWORD error=ok?ERROR_SUCCESS:GetLastError();
            if(!ok&&error==ERROR_IO_PENDING){
                auto now=GetTickCount64();DWORD left=now<deadline?DWORD(deadline-now):0;
                if(WaitForSingleObject(ov.hEvent,left)!=WAIT_OBJECT_0){
                    CancelIoEx(pipe,&ov);GetOverlappedResult(pipe,&ov,&done,TRUE);CloseHandle(ov.hEvent);
                    throw std::runtime_error("Encoding worker timed out");
                }
                ok=GetOverlappedResult(pipe,&ov,&done,FALSE);
            }
            CloseHandle(ov.hEvent);
            if(!ok||!done)throw std::runtime_error("Encoding worker disconnected");
            p+=done;size-=done;
        }
    }
    wire::Bytes request(uint32_t kind,const wire::Bytes& request,DWORD timeout=8000){
        wire::Header h;h.kind=kind;h.length=uint32_t(request.size());
        if(request.size()>wire::limit)throw std::runtime_error("Frame exceeds worker message limit");
        io(&h,sizeof(h),true,timeout);
        if(!request.empty())io(const_cast<uint8_t*>(request.data()),request.size(),true,timeout);
        io(&h,sizeof(h),false,timeout);
        if(h.magic_value!=wire::magic||h.length>wire::limit)throw std::runtime_error("Invalid worker response");
        wire::Bytes response(h.length);
        if(!response.empty())io(response.data(),response.size(),false,timeout);
        if(h.kind==wire::Failure){wire::Reader r{response};throw std::runtime_error(r.text());}
        if(h.kind!=wire::Result)throw std::runtime_error("Unexpected worker response");
        return response;
    }
};
}
std::vector<Capability> worker_capabilities(){
    Channel channel;auto b=channel.request(wire::Probe,{},20000);wire::Reader r{b};
    uint32_t n=r.get<uint32_t>();if(n>16)throw std::runtime_error("Too many worker adapters");
    std::vector<Capability> values;
    for(uint32_t k=0;k<n;++k){
        Capability c;c.adapter=wire::get_adapter(r);c.mask=r.get<uint32_t>();
        for(auto& reason:c.reasons)reason=r.text();
        values.push_back(std::move(c));
    }
    r.end();return values;
}
struct WorkerEncoder::Impl {Channel channel;};
WorkerEncoder::WorkerEncoder() {}
WorkerEncoder::~WorkerEncoder(){
    if(impl_){try{impl_->channel.request(wire::Stop,{},1000);}catch(...){}}
}
bool WorkerEncoder::open(const Adapter& a,const Codec& c,const Settings& s,std::string& error){
    try{
        settings_=s;impl_.reset(new Impl);
        wire::Bytes b;wire::put_adapter(b,a);wire::text(b,c.id);wire::put_settings(b,s);
        auto response=impl_->channel.request(wire::Init,b,15000);wire::Reader r{response};
        extra_=r.bytes(r.get<uint32_t>());r.end();
        if(extra_.empty())throw std::runtime_error("Worker returned an empty codec header");
        return true;
    }catch(const std::exception& e){error=e.what();impl_.reset();return false;}
}
bool WorkerEncoder::send(const uint8_t* y,int py,const uint8_t* uv,int puv,int64_t pts,std::string& error){
    try{
        if(!impl_||!y||!uv||py<settings_.width||puv<settings_.width)throw std::runtime_error("Invalid worker frame");
        size_t pixels=size_t(settings_.width)*settings_.height;
        wire::Bytes b(sizeof(pts)+pixels*3/2);std::memcpy(b.data(),&pts,sizeof(pts));
        uint8_t* dest=b.data()+sizeof(pts);
        for(int row=0;row<settings_.height;++row)std::memcpy(dest+size_t(row)*settings_.width,y+size_t(row)*py,settings_.width);
        dest+=pixels;
        for(int row=0;row<settings_.height/2;++row)std::memcpy(dest+size_t(row)*settings_.width,uv+size_t(row)*puv,settings_.width);
        auto response=impl_->channel.request(wire::Frame,b);wire::Reader r{response};
        for(auto& packet:wire::get_packets(r)){packets_.push_back(std::move(packet));++packet_count_;}
        if(packets_.size()>240)throw std::runtime_error("Worker packet queue exceeded");
        return true;
    }catch(const std::exception& e){error=e.what();return false;}
}
bool WorkerEncoder::flush(std::string& error){
    try{
        auto response=impl_->channel.request(wire::Flush,{});wire::Reader r{response};
        for(auto& packet:wire::get_packets(r)){packets_.push_back(std::move(packet));++packet_count_;}
        return true;
    }catch(const std::exception& e){error=e.what();return false;}
}
bool WorkerEncoder::receive(Packet& packet){
    if(packets_.empty())return false;packet=std::move(packets_.front());packets_.pop_front();return true;
}
}
