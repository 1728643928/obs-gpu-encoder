// SPDX-License-Identifier: GPL-2.0-or-later
// Tests use disposable fixtures, not the user's OBS installation.
import fs from 'node:fs/promises';
import path from 'node:path';
import assert from 'node:assert/strict';
import {spawn,spawnSync} from 'node:child_process';
import {fileURLToPath} from 'node:url';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const args=process.argv.slice(2),at=args.indexOf('--obs-root');
const obs=at<0?path.join(root,'build/_deps/obs-runtime'):path.resolve(args[at+1]);
const {version}=JSON.parse(await fs.readFile(path.join(root,'version.json'),'utf8'));
const setup=path.join(root,'release','GPU-Encoder-Bridge-'+version+'-windows-x64-setup.exe');
const base=path.join(root,'build/installer-tests');await fs.mkdir(base,{recursive:true});
const run=(exe,argv,expected=0)=>{const r=spawnSync(exe,argv,{cwd:base,windowsHide:true,encoding:'utf8',timeout:30000});assert.ifError(r.error);assert.equal(r.status,expected,'Unexpected installer exit '+r.status+' '+r.stderr);};
for(const lang of ['en','zh','ja','ko']){
 const dir=path.join(base,'OBS fixture '+lang+' 测试');
 await fs.mkdir(path.join(dir,'bin/64bit'),{recursive:true});
 await fs.copyFile(path.join(obs,'bin/64bit/obs64.exe'),path.join(dir,'bin/64bit/obs64.exe'));
 // Version comes from a real OBS executable. DLL placeholders are never loaded.
 for(const dll of ['obs.dll','obs-frontend-api.dll','avcodec-62.dll','avutil-60.dll'])await fs.writeFile(path.join(dir,'bin/64bit',dll),'fixture only');
 await fs.mkdir(path.join(dir,'obs-plugins/64bit'),{recursive:true});
 const sentinel=path.join(dir,'obs-plugins/64bit/unrelated-plugin.txt');await fs.writeFile(sentinel,'keep');
 const argv=['/VERYSILENT','/SUPPRESSMSGBOXES','/NORESTART','/CURRENTUSER','/LANG='+lang,'/DIR='+dir];
 run(setup,argv);
 for(const file of ['obs-gpu-encoder.dll','obs-gpu-encoder-worker.exe'])assert((await fs.stat(path.join(dir,'obs-plugins/64bit',file))).size>10000);
 for(const locale of ['en-US','zh-CN','ja-JP','ko-KR'])assert((await fs.stat(path.join(dir,'data/obs-plugins/obs-gpu-encoder/locale',locale+'.ini'))).size>100);
 const extra=path.join(dir,'data/obs-plugins/obs-gpu-encoder/user-note.txt');await fs.writeFile(extra,'keep user file');
 run(path.join(dir,'data/obs-plugins/obs-gpu-encoder/uninstall/unins000.exe'),['/VERYSILENT','/SUPPRESSMSGBOXES','/NORESTART']);
 await assert.rejects(fs.access(path.join(dir,'obs-plugins/64bit/obs-gpu-encoder.dll')));
 assert.equal(await fs.readFile(sentinel,'utf8'),'keep');assert.equal(await fs.readFile(extra,'utf8'),'keep user file');
 assert((await fs.stat(path.join(dir,'bin/64bit/obs64.exe'))).size>100000);
 console.log('PASS '+lang+' install/uninstall; four locales; unrelated files retained');
}
const invalid=path.join(base,'not OBS');await fs.mkdir(invalid,{recursive:true});
const fail=spawnSync(setup,['/VERYSILENT','/SUPPRESSMSGBOXES','/NORESTART','/CURRENTUSER','/DIR='+invalid],{windowsHide:true,timeout:15000});assert(fail.status!==0);await assert.rejects(fs.access(path.join(invalid,'obs-plugins/64bit/obs-gpu-encoder.dll')));console.log('PASS invalid OBS root rejected without plugin files');
const locked=path.join(base,'OBS fixture en 测试');
const child=spawn(path.join(root,'build/lockfile.exe'),[path.join(locked,'bin/64bit/obs.dll')],{windowsHide:true,stdio:['pipe','pipe','pipe']});
try{
 await new Promise((resolve,reject)=>{child.once('error',reject);child.stdout.once('data',data=>data.toString().includes('LOCKED')?resolve():reject(Error('Lock fixture failed')));child.once('exit',code=>{if(code)reject(Error('Lock fixture exit '+code));});});
 const blocked=spawnSync(setup,['/VERYSILENT','/SUPPRESSMSGBOXES','/NORESTART','/CURRENTUSER','/DIR='+locked],{windowsHide:true,timeout:15000});assert(blocked.status!==0);await assert.rejects(fs.access(path.join(locked,'obs-plugins/64bit/obs-gpu-encoder.dll')));console.log('PASS in-use target rejected before copying plugin');
}finally{child.stdin.end('\n');}
