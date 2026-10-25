// SPDX-License-Identifier: GPL-2.0-or-later
#include "protocol.hpp"
#include <cstdio>
#include <stdexcept>
int main(){
 using namespace gbe::wire;
 try{
  Bytes b;text(b,"GPU \xe6\xa0\xb8\xe6\x98\xbe");put(b,int64_t(123456789));
  Reader r{b};if(r.text()!="GPU \xe6\xa0\xb8\xe6\x98\xbe"||r.get<int64_t>()!=123456789)return 1;r.end();
  gbe::Settings s;s.width=1280;s.full_range=true;s.quality="quality";Bytes cfg;put_settings(cfg,s);
  Reader sr{cfg};auto result=get_settings(sr);sr.end();
  if(result.width!=1280||!result.full_range||result.quality!="quality")return 2;
  bool malformed=false;try{Bytes truncated={255,255,255,127};Reader tr{truncated};tr.text();}catch(const std::exception&){malformed=true;}
  bool oversized=false;try{Bytes huge;put(huge,uint32_t(241));Reader hr{huge};get_packets(hr);}catch(const std::exception&){oversized=true;}
  bool trailing=false;try{Bytes tail={1};Reader tr{tail};tr.end();}catch(const std::exception&){trailing=true;}
  if(!malformed||!oversized||!trailing)return 3;
  puts("PASS protocol roundtrip, UTF-8, truncation, size bounds and trailing data");
  return 0;
 }catch(const std::exception& e){fprintf(stderr,"%s\n",e.what());return 4;}
}
