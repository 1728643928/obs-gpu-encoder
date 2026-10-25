// SPDX-License-Identifier: GPL-2.0-or-later
import fs from 'node:fs/promises';
import path from 'node:path';
import crypto from 'node:crypto';
import {spawnSync} from 'node:child_process';
import {fileURLToPath} from 'node:url';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const args=process.argv.slice(2);
const at=args.indexOf('--compiler');
let compiler=at<0?null:args[at+1];
const cache=path.join(root,'build/_deps/inno-6.7.3');
const hash='9c73c3bae7ed48d44112a0f48e66742c00090bdb5bef71d9d3c056c66e97b732';
function run(exe,argv){const r=spawnSync(exe,argv,{cwd:root,stdio:'inherit',windowsHide:true});if(r.error||r.status!==0)throw r.error||Error('Command failed: '+r.status);}
if(!compiler){
 compiler=path.join(cache,'ISCC.exe');
 try{await fs.access(compiler);}catch{
  await fs.mkdir(cache,{recursive:true});
  const response=await fetch('https://github.com/jrsoftware/issrc/releases/download/is-6_7_3/innosetup-6.7.3.exe');
  if(!response.ok)throw Error('Compiler download failed: '+response.status);
  const bytes=Buffer.from(await response.arrayBuffer());
  if(crypto.createHash('sha256').update(bytes).digest('hex')!==hash)throw Error('Compiler SHA-256 mismatch');
  const setup=path.join(cache,'compiler-setup.exe');await fs.writeFile(setup,bytes);
  run(setup,['/VERYSILENT','/SUPPRESSMSGBOXES','/NORESTART','/CURRENTUSER','/NOICONS','/TASKS=','/DIR='+cache]);
 }
}
const {version}=JSON.parse(await fs.readFile(path.join(root,'version.json'),'utf8'));
if(!/^\d+\.\d+\.\d+(?:-[a-z0-9.-]+)?$/.test(version))throw Error('Invalid version');
const repo=process.env.GITHUB_REPOSITORY||'1728643928/obs-gpu-encoder';
if(!/^[A-Za-z0-9_.-]+\/[A-Za-z0-9_.-]+$/.test(repo))throw Error('Invalid repository');
run(compiler,['/Qp','/DAppVersion='+version,'/DProjectURL=https://github.com/'+repo,path.join(root,'installer/setup.iss')]);
console.log('Built four-language installer for '+version);
