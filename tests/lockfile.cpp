// SPDX-License-Identifier: GPL-2.0-or-later
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdio>
int wmain(int argc,wchar_t**argv){
 if(argc!=2)return 2;
 HANDLE h=CreateFileW(argv[1],GENERIC_READ,0,nullptr,OPEN_EXISTING,0,nullptr);
 if(h==INVALID_HANDLE_VALUE)return 3;
 puts("LOCKED");fflush(stdout);getchar();CloseHandle(h);return 0;
}
