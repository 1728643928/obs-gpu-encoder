// SPDX-License-Identifier: GPL-2.0-or-later
#define NOMINMAX
#include <windows.h>
#include <obs-module.h>
#include <obs-frontend-api.h>
#include <util/config-file.h>
#include "enhanced-output.hpp"
#include <algorithm>
#include <memory>
#include <vector>
namespace gbe {
namespace {
std::vector<EnhancedBinding> bindings;
std::unique_ptr<EnhancedRoute> route;
constexpr const char* section="GPUEncoderBridge";
std::string profile_string(config_t* p,const char* key){const char* v=config_get_string(p,section,key);return v?v:"";}
std::wstring wide(const char* value) {
    int n=MultiByteToWideChar(CP_UTF8,0,value,-1,nullptr,0);
    std::wstring out(n?size_t(n):1,L'\0');
    if(n)MultiByteToWideChar(CP_UTF8,0,value,-1,out.data(),n);
    out.pop_back();return out;
}
struct Dialog {
    std::vector<const EnhancedBinding*> adapters;
    config_t* profile=nullptr;
};
enum {Enable=100,Devices=101};
void word(std::vector<WORD>& b,WORD v){b.push_back(v);}
void text(std::vector<WORD>& b,const std::wstring& v){for(auto c:v)b.push_back(WORD(c));b.push_back(0);}
void control(std::vector<WORD>& b,DWORD style,short x,short y,short w,short h,WORD id,WORD cls,const std::wstring& label){
    if(b.size()%2)b.push_back(0);
    DLGITEMTEMPLATE item={};item.style=WS_CHILD|WS_VISIBLE|style;item.x=x;item.y=y;item.cx=w;item.cy=h;item.id=id;
    auto p=reinterpret_cast<const WORD*>(&item);b.insert(b.end(),p,p+sizeof(item)/2);
    word(b,0xffff);word(b,cls);text(b,label);word(b,0);
}
std::vector<WORD> dialog_template() {
    DLGTEMPLATE d={};d.style=WS_POPUP|WS_CAPTION|WS_SYSMENU|DS_MODALFRAME|DS_SETFONT;d.cdit=6;d.cx=360;d.cy=156;
    std::vector<WORD> b;auto p=reinterpret_cast<const WORD*>(&d);b.insert(b.end(),p,p+sizeof(d)/2);
    word(b,0);word(b,0);text(b,wide(obs_module_text("EnhancedTitle")));word(b,9);text(b,L"Segoe UI");
    control(b,BS_AUTOCHECKBOX|WS_TABSTOP,12,12,336,16,Enable,0x80,wide(obs_module_text("EnhancedEnable")));
    control(b,SS_LEFT,12,36,336,12,110,0x82,wide(obs_module_text("Device")));
    control(b,CBS_DROPDOWNLIST|WS_VSCROLL|WS_TABSTOP,12,52,336,120,Devices,0x85,L"");
    control(b,SS_LEFT,12,78,336,42,111,0x82,wide(obs_module_text("EnhancedHelp")));
    control(b,BS_DEFPUSHBUTTON|WS_TABSTOP,194,130,74,16,IDOK,0x80,wide(obs_module_text("EnhancedSave")));
    control(b,BS_PUSHBUTTON|WS_TABSTOP,274,130,74,16,IDCANCEL,0x80,wide(obs_module_text("EnhancedCancel")));
    return b;
}
INT_PTR CALLBACK dialog_proc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp) {
    auto d=reinterpret_cast<Dialog*>(GetWindowLongPtrW(hwnd,DWLP_USER));
    if(msg==WM_INITDIALOG) {
        d=reinterpret_cast<Dialog*>(lp);SetWindowLongPtrW(hwnd,DWLP_USER,lp);
        CheckDlgButton(hwnd,Enable,config_get_bool(d->profile,section,"ForceEnhancedGPU")?BST_CHECKED:BST_UNCHECKED);
        std::string selected=profile_string(d->profile,"EnhancedAdapter");
        int index=0,chosen=-1;
        for(auto b:d->adapters){
            auto label=wide((b->adapter_name+" [GPU "+std::to_string(b->adapter_index)+"]").c_str());
            SendDlgItemMessageW(hwnd,Devices,CB_ADDSTRING,0,reinterpret_cast<LPARAM>(label.c_str()));
            if(b->adapter_identity==selected)chosen=index;
            ++index;
        }
        // Missing saved hardware requires an explicit fresh selection.
        SendDlgItemMessageW(hwnd,Devices,CB_SETCURSEL,chosen,0);
        return TRUE;
    }
    if(msg==WM_COMMAND&&LOWORD(wp)==IDCANCEL){EndDialog(hwnd,IDCANCEL);return TRUE;}
    if(msg==WM_COMMAND&&LOWORD(wp)==IDOK&&d) {
        if(obs_frontend_streaming_active()){
            MessageBoxW(hwnd,wide(obs_module_text("EnhancedStopFirst")).c_str(),wide(obs_module_text("EnhancedTitle")).c_str(),MB_OK|MB_ICONINFORMATION);return TRUE;
        }
        bool enabled=IsDlgButtonChecked(hwnd,Enable)==BST_CHECKED;
        auto index=SendDlgItemMessageW(hwnd,Devices,CB_GETCURSEL,0,0);
        if(enabled&&(index<0||size_t(index)>=d->adapters.size())){
            MessageBoxW(hwnd,wide(obs_module_text("EnhancedSelectGPU")).c_str(),wide(obs_module_text("EnhancedTitle")).c_str(),MB_OK|MB_ICONWARNING);
            return TRUE;
        }
        bool old_enabled=config_get_bool(d->profile,section,"ForceEnhancedGPU");
        std::string old_adapter=profile_string(d->profile,"EnhancedAdapter");
        config_set_bool(d->profile,section,"ForceEnhancedGPU",enabled);
        if(index>=0&&size_t(index)<d->adapters.size())
            config_set_string(d->profile,section,"EnhancedAdapter",d->adapters[size_t(index)]->adapter_identity.c_str());
        if(config_save_safe(d->profile,"tmp",nullptr)!=CONFIG_SUCCESS){
            config_set_bool(d->profile,section,"ForceEnhancedGPU",old_enabled);
            config_set_string(d->profile,section,"EnhancedAdapter",old_adapter.c_str());
            MessageBoxW(hwnd,wide(obs_module_text("EnhancedSaveFailed")).c_str(),wide(obs_module_text("EnhancedTitle")).c_str(),MB_OK|MB_ICONERROR);
            return TRUE;
        }
        EndDialog(hwnd,IDOK);return TRUE;
    }
    return FALSE;
}
void settings(void*) {
    if(obs_frontend_streaming_active()){
        MessageBoxW(static_cast<HWND>(obs_frontend_get_main_window_handle()),wide(obs_module_text("EnhancedStopFirst")).c_str(),
                    wide(obs_module_text("EnhancedTitle")).c_str(),MB_OK|MB_ICONINFORMATION);return;
    }
    Dialog d;d.profile=obs_frontend_get_profile_config();if(!d.profile)return;
    for(const auto& b:bindings)if(std::none_of(d.adapters.begin(),d.adapters.end(),[&](const EnhancedBinding* v){return v->adapter_identity==b.adapter_identity;}))
        d.adapters.push_back(&b);
    auto resource=dialog_template();
    if(DialogBoxIndirectParamW(GetModuleHandleW(nullptr),reinterpret_cast<DLGTEMPLATE*>(resource.data()),
       static_cast<HWND>(obs_frontend_get_main_window_handle()),dialog_proc,reinterpret_cast<LPARAM>(&d))==-1)
        blog(LOG_ERROR,"[gpu-encoder] Could not show enhanced GPU settings: %lu",GetLastError());
}
void event(obs_frontend_event event,void*) {
    if(event==OBS_FRONTEND_EVENT_STREAMING_STOPPED){route.reset();return;}
    if(event==OBS_FRONTEND_EVENT_EXIT){route.reset();return;}
    if(event!=OBS_FRONTEND_EVENT_STREAMING_STARTING)return;
    auto profile=obs_frontend_get_profile_config();if(!profile)return;
    if(!config_get_bool(profile,section,"ForceEnhancedGPU")||!config_get_bool(profile,"Stream1","EnableMultitrackVideo"))return;
    // At this event OBS has prepared its multitrack output but has not started it.
    auto output=obs_frontend_get_streaming_output();
    if(!output){blog(LOG_ERROR,"[gpu-encoder] ENHANCED output unavailable");return;}
    route.reset();route=std::make_unique<EnhancedRoute>();std::string error;
    route_enhanced_output(output,profile_string(profile,"EnhancedAdapter"),bindings,*route,error);
    obs_output_release(output);
}
}
void start_enhanced_frontend(std::vector<EnhancedBinding> values) {
    bindings=std::move(values);register_enhanced_guard();
    obs_frontend_add_tools_menu_item(obs_module_text("EnhancedMenu"),settings,nullptr);
    obs_frontend_add_event_callback(event,nullptr);
}
void stop_enhanced_frontend(){obs_frontend_remove_event_callback(event,nullptr);route.reset();}
}
