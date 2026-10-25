// SPDX-License-Identifier: GPL-2.0-or-later
// Node.js 20+; uses official pinned sources and a portable Zig toolchain.
import fs from 'node:fs/promises';
import path from 'node:path';
import crypto from 'node:crypto';
import {spawnSync} from 'node:child_process';
import {fileURLToPath} from 'node:url';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const args=process.argv.slice(2);
const version=JSON.parse(await fs.readFile(path.join(root,'version.json'),'utf8')).version;
if(!/^\d+\.\d+\.\d+(?:-[a-z0-9.-]+)?$/.test(version))throw Error('Invalid version');
const value=(name,fallback)=>{const i=args.indexOf(name);return i<0?fallback:args[i+1];};
if(process.platform!=='win32')throw Error('This preview builds Windows x64 only.');
const cache=path.resolve(value('--deps-dir',path.join(root,'build/_deps')));
const output=path.join(root,'dist');
await fs.mkdir(cache,{recursive:true});
for(const d of ['build/lib','build/include','build/tmp','dist/obs-plugins/64bit','dist/data/obs-plugins/obs-gpu-encoder/locale'])
    await fs.mkdir(path.join(root,d),{recursive:true});
const deps=[
 {name:'zig',folder:'zig-x86_64-windows-0.14.1',url:'https://ziglang.org/download/0.14.1/zig-x86_64-windows-0.14.1.zip',hash:'554f5378228923ffd558eac35e21af020c73789d87afeabf4bfd16f2e6feed2c'},
 {name:'obs',folder:'obs-studio-33.0.0',url:'https://codeload.github.com/obsproject/obs-studio/zip/refs/tags/33.0.0',hash:'bdab14146289f39be6c12d5165812ba5b37b5231f857cb84aa5e4bafeaaa8686'},
 {name:'ffmpeg',folder:'FFmpeg-n8.0',url:'https://codeload.github.com/FFmpeg/FFmpeg/zip/refs/tags/n8.0',hash:'775db720f4821967c360b7cecb277540a9eb8d72a9af1cd7c1d2849c4c8c881c'}
];
function run(exe,argv,env=process.env){
 const r=spawnSync(exe,argv,{cwd:root,env,stdio:'inherit',windowsHide:true});
 if(r.error||r.status!==0)throw r.error||Error(exe+' failed: '+r.status);
}
async function download(url,dest,hash){
 let bytes;
 try{bytes=await fs.readFile(dest);}catch{}
 if(!bytes||crypto.createHash('sha256').update(bytes).digest('hex')!==hash){
  console.log('Downloading '+path.basename(dest));
  const response=await fetch(url);
  if(!response.ok)throw Error('HTTP '+response.status+' downloading '+url);
  bytes=Buffer.from(await response.arrayBuffer());
  if(crypto.createHash('sha256').update(bytes).digest('hex')!==hash)throw Error('SHA-256 mismatch: '+url);
  await fs.writeFile(dest,bytes);
 }
}
for(const dep of deps){
 try{await fs.access(path.join(cache,dep.folder));}
 catch{const archive=path.join(cache,dep.name+'.zip');await download(dep.url,archive,dep.hash);
       run('tar.exe',['-xf',archive,'-C',cache]);}
}
let obsRoot=value('--obs-root',null);
if(!obsRoot&&args.includes('--download-obs')){
 obsRoot=path.join(cache,'obs-runtime');
 try{await fs.access(path.join(obsRoot,'bin/64bit/obs.dll'));}
 catch{await fs.mkdir(obsRoot,{recursive:true});const archive=path.join(cache,'obs-runtime.zip');
  await download('https://github.com/obsproject/obs-studio/releases/download/33.0.0/OBS-Studio-33.0.0-Windows-x64.zip',archive,'3369a3ecd67b5669a8a08d77357f87cc786d1fba42fbbd1054d841a69f6271a7');
  run('tar.exe',['-xf',archive,'-C',obsRoot]);}
}
if(!obsRoot)throw Error('Pass --obs-root "C:\\Program Files\\obs-studio" or --download-obs.');
obsRoot=path.resolve(obsRoot);
const binaries=path.join(obsRoot,'bin/64bit');
function peExports(b){
 const pe=b.readUInt32LE(0x3c),opt=pe+24,n=b.readUInt16LE(pe+6),table=opt+b.readUInt16LE(pe+20);
 if(b.readUInt16LE(opt)!==0x20b)throw Error('Expected PE32+ DLL');
 const rva=v=>{for(let i=0;i<n;i++){const p=table+i*40,va=b.readUInt32LE(p+12);
  if(v>=va&&v<va+Math.max(b.readUInt32LE(p+8),b.readUInt32LE(p+16)))return b.readUInt32LE(p+20)+v-va;}throw Error('Invalid DLL RVA');};
 const dir=rva(b.readUInt32LE(opt+112)),count=b.readUInt32LE(dir+24),names=rva(b.readUInt32LE(dir+32)),result=[];
 for(let i=0;i<count;i++){const p=rva(b.readUInt32LE(names+i*4));result.push(b.toString('ascii',p,b.indexOf(0,p)));}
 return result;
}
const zig=path.join(cache,deps[0].folder,'zig.exe');
const env={...process.env,ZIG_GLOBAL_CACHE_DIR:path.join(cache,'zig-cache'),TEMP:path.join(root,'build/tmp'),TMP:path.join(root,'build/tmp')};
for(const dll of ['obs.dll','avcodec-62.dll','avutil-60.dll']){
 const exports=peExports(await fs.readFile(path.join(binaries,dll)));
 const def=path.join(root,'build/lib',dll+'.def');
 await fs.writeFile(def,'LIBRARY '+dll+'\nEXPORTS\n'+exports.map(s=>'  '+s).join('\n'));
 run(zig,['dlltool','-d',def,'-l',path.join(root,'build/lib',dll+'.a'),'-m','i386:x86-64'],env);
}
const ffmpeg=path.join(cache,deps[2].folder),obs=path.join(cache,deps[1].folder,'libobs');
await fs.writeFile(path.join(ffmpeg,'libavutil/avconfig.h'),'#pragma once\n#define AV_HAVE_BIGENDIAN 0\n#define AV_HAVE_FAST_UNALIGNED 1\n');
await fs.writeFile(path.join(root,'build/include/obsconfig.h'),'#pragma once\n#define OBS_DATA_PATH "data"\n#define OBS_PLUGIN_PATH "obs-plugins"\n#define OBS_PLUGIN_DESTINATION "obs-plugins"\n#define OBS_RELEASE_CANDIDATE 0\n#define OBS_BETA 0\n');
await fs.writeFile(path.join(root,'build/include/version.hpp'),'#pragma once\n#define GBE_VERSION "'+version+'"\n');
const common=['c++','-target','x86_64-windows-gnu','-std=c++17','-O2','-g0','-static','-ffile-prefix-map='+root+'=.','-Isrc','-Ibuild/include','-I'+ffmpeg];
const libraries=['build/lib/avcodec-62.dll.a','build/lib/avutil-60.dll.a','-ld3d11','-ldxgi'];
run(zig,[...common,'-c','src/backend.cpp','-o','build/backend.obj'],env);
run(zig,[...common,'-s','build/backend.obj','src/worker.cpp',...libraries,'-o','dist/obs-plugins/64bit/obs-gpu-encoder-worker.exe'],env);
run(zig,[...common,'-s','-I'+obs,'build/backend.obj','src/plugin.cpp','src/worker-client.cpp','-shared','build/lib/obs.dll.a',...libraries,'-o','dist/obs-plugins/64bit/obs-gpu-encoder.dll'],env);
for(const locale of ['en-US','zh-CN','ja-JP','ko-KR'])
 await fs.copyFile(path.join(root,'data/locale',locale+'.ini'),path.join(output,'data/obs-plugins/obs-gpu-encoder/locale',locale+'.ini'));
for(const name of ['README.md','LICENSE','THIRD-PARTY.md'])await fs.copyFile(path.join(root,name),path.join(output,name));
await fs.cp(path.join(root,'third-party'),path.join(output,'third-party'),{recursive:true});
if(args.includes('--tests')){
 run(zig,[...common,'-municode','tests/lockfile.cpp','-o','build/lockfile.exe'],env);
 run(zig,[...common,'-municode','build/backend.obj','tests/worker-smoke.cpp','src/worker-client.cpp',...libraries,'-o','build/worker-smoke.exe'],env);
 run(zig,[...common,'build/backend.obj','tests/backend-smoke.cpp',...libraries,'-o','build/backend-smoke.exe'],env);
 run(zig,[...common,'tests/protocol-test.cpp','-o','build/protocol-test.exe'],env);
 run(path.join(root,'build/protocol-test.exe'),[],env);
 run(zig,[...common,'tests/adapter-policy-test.cpp','-o','build/adapter-policy-test.exe'],env);
 run(path.join(root,'build/adapter-policy-test.exe'),[],env);
}
console.log('Built '+output+'\nNo OBS configuration or installed files were changed.');
