// SPDX-License-Identifier: GPL-2.0-or-later
import fs from 'node:fs/promises';
import path from 'node:path';
import crypto from 'node:crypto';
import {spawnSync} from 'node:child_process';
import {fileURLToPath} from 'node:url';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const {version}=JSON.parse(await fs.readFile(path.join(root,'version.json'),'utf8'));
if(!/^\d+\.\d+\.\d+(?:-[a-z0-9.-]+)?$/.test(version))throw Error('Invalid version');
const output=path.join(root,'release');await fs.mkdir(output,{recursive:true});
const prefix='GPU-Encoder-Bridge-'+version;
const git=(...args)=>{const r=spawnSync('git',args,{cwd:root,encoding:'utf8',windowsHide:true});if(r.error||r.status!==0)throw r.error||Error(r.stderr);return r.stdout.trim();};
const source=git('ls-files','-z').split('\0').filter(Boolean).sort();
for(const required of ['version.json','installer/setup.iss','scripts/build.mjs','src/plugin.cpp','LICENSE'])if(!source.includes(required))throw Error('Stage all source files before packaging: '+required);
for(const name of source)if(/^(?:build|dist|release|work)\/|\.(?:dll|exe|pdb|log|flv|mkv|h264)$/i.test(name))throw Error('Unexpected file in source: '+name);
const table=Array.from({length:256},(_,i)=>{for(let j=0;j<8;j++)i=(i&1)?0xedb88320^(i>>>1):i>>>1;return i>>>0;});
function crc(bytes){let c=0xffffffff;for(const b of bytes)c=table[(c^b)&255]^(c>>>8);return (c^0xffffffff)>>>0;}
async function zip(destination,entries){
 const chunks=[],central=[];let offset=0;
 for(const [name,filename] of entries){
  if(name.includes('..')||name.startsWith('/')||name.includes('\\'))throw Error('Unsafe ZIP name');
  const data=await fs.readFile(filename),n=Buffer.from(name),sum=crc(data);
  const local=Buffer.alloc(30);local.writeUInt32LE(0x04034b50);local.writeUInt16LE(20,4);local.writeUInt16LE(0x800,6);local.writeUInt16LE(33,12);local.writeUInt32LE(sum,14);local.writeUInt32LE(data.length,18);local.writeUInt32LE(data.length,22);local.writeUInt16LE(n.length,26);
  chunks.push(local,n,data);const c=Buffer.alloc(46);c.writeUInt32LE(0x02014b50);c.writeUInt16LE(20,4);c.writeUInt16LE(20,6);c.writeUInt16LE(0x800,8);c.writeUInt16LE(33,14);c.writeUInt32LE(sum,16);c.writeUInt32LE(data.length,20);c.writeUInt32LE(data.length,24);c.writeUInt16LE(n.length,28);c.writeUInt32LE(offset,42);central.push(c,n);offset+=local.length+n.length+data.length;
 }
 const cd=Buffer.concat(central),end=Buffer.alloc(22);end.writeUInt32LE(0x06054b50);end.writeUInt16LE(entries.length,8);end.writeUInt16LE(entries.length,10);end.writeUInt32LE(cd.length,12);end.writeUInt32LE(offset,16);await fs.writeFile(destination,Buffer.concat([...chunks,cd,end]));
}
async function walk(dir,base=dir){let found=[];for(const e of await fs.readdir(dir,{withFileTypes:true})){const file=path.join(dir,e.name);if(e.isSymbolicLink())throw Error('Unexpected symbolic link');if(e.isDirectory())found.push(...await walk(file,base));else found.push([path.relative(base,file).replaceAll('\\','/'),file]);}return found.sort(([a],[b])=>a.localeCompare(b));}
await zip(path.join(output,prefix+'-source.zip'),source.map(s=>[s,path.join(root,s)]));
await zip(path.join(output,prefix+'-windows-x64.zip'),await walk(path.join(root,'dist')));
const exe=prefix+'-windows-x64-setup.exe';await fs.access(path.join(output,exe));
const assets=[exe,prefix+'-windows-x64.zip',prefix+'-source.zip'];
const files=[];for(const file of assets){const bytes=await fs.readFile(path.join(output,file));files.push({file,size:bytes.length,sha256:crypto.createHash('sha256').update(bytes).digest('hex')});}
await fs.writeFile(path.join(output,'SHA256SUMS.txt'),files.map(f=>f.sha256+'  '+f.file).join('\n')+'\n');
await fs.writeFile(path.join(output,'build-info.json'),JSON.stringify({version,commit:git('rev-parse','HEAD'),repository:process.env.GITHUB_REPOSITORY||'1728643928/obs-gpu-encoder',sourceFiles:source.length,files},null,2)+'\n');
console.log('Packaged '+version+' with '+source.length+' source files');
