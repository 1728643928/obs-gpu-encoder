// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include "backend.hpp"
#include <array>
namespace gbe {
void set_worker_location(const std::wstring& executable,const std::wstring& directory);
struct Capability {Adapter adapter;uint32_t mask=0;std::array<std::string,3> reasons;};
std::vector<Capability> worker_capabilities();
class WorkerEncoder {
public:
    WorkerEncoder();~WorkerEncoder();
    WorkerEncoder(const WorkerEncoder&)=delete;
    WorkerEncoder& operator=(const WorkerEncoder&)=delete;
    bool open(const Adapter&,const Codec&,const Settings&,std::string&);
    bool send(const uint8_t*,int,const uint8_t*,int,int64_t,std::string&);
    bool flush(std::string&);
    bool receive(Packet&);
    const std::vector<uint8_t>& header() const{return extra_;}
    unsigned encoded_packets() const{return packet_count_;}
private:
    struct Impl;std::unique_ptr<Impl> impl_;
    Settings settings_;std::vector<uint8_t> extra_;std::deque<Packet> packets_;
    unsigned packet_count_=0;
};
}
