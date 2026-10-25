// SPDX-License-Identifier: GPL-2.0-or-later
// Run only in a trusted GitHub Actions publish job. Never print the token.
import fs from 'node:fs/promises';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const {version}=JSON.parse(await fs.readFile(path.join(root,'version.json'),'utf8'));
const repository=process.env.GITHUB_REPOSITORY,sha=process.env.GITHUB_SHA,token=process.env.GITHUB_TOKEN;
if(!token||!repository||!/^[A-Za-z0-9_.-]+\/[A-Za-z0-9_.-]+$/.test(repository)||!sha||!/^[a-f0-9]{40}$/.test(sha))throw Error('Use the trusted GitHub Actions release job');
if(!/^\d+\.\d+\.\d+(?:-[a-z0-9.-]+)?$/.test(version))throw Error('Invalid version');
const tag='v'+version,base='https://api.github.com/repos/'+repository;
const headers={Authorization:'Bearer '+token,Accept:'application/vnd.github+json','X-GitHub-Api-Version':'2022-11-28'};
async function request(url,method='GET',body){const r=await fetch(url,{method,headers:{...headers,'Content-Type':'application/json'},body:body===undefined?undefined:JSON.stringify(body)});if(!r.ok)throw Error('GitHub API '+r.status+' '+method);return r.json();}
const releaseFiles=['GPU-Encoder-Bridge-'+version+'-windows-x64-setup.exe','GPU-Encoder-Bridge-'+version+'-windows-x64.zip','GPU-Encoder-Bridge-'+version+'-source.zip','SHA256SUMS.txt','build-info.json'];
for(const file of releaseFiles)await fs.access(path.join(root,'release',file));
const existing=await fetch(base+'/releases/tags/'+encodeURIComponent(tag),{headers});
if(existing.ok)throw Error('Release already exists. Increment version.json; published assets are never overwritten.');
if(existing.status!==404)throw Error('Cannot check existing release: '+existing.status);
const notes=await fs.readFile(path.join(root,'docs/release-notes.md'),'utf8');
const release=await request(base+'/releases','POST',{tag_name:tag,target_commitish:sha,name:'GPU Encoder Bridge '+version,body:notes,draft:true,prerelease:version.includes('-')});
console.log('Created draft release '+tag);
for(const file of releaseFiles){
 const bytes=await fs.readFile(path.join(root,'release',file));
 const url=release.upload_url.split('{')[0]+'?name='+encodeURIComponent(file);
 const r=await fetch(url,{method:'POST',headers:{...headers,'Content-Type':'application/octet-stream'},body:bytes});
 if(!r.ok)throw Error('Asset upload failed: '+r.status+'; draft retained for inspection');
 console.log('Uploaded '+file);
}
const published=await request(base+'/releases/'+release.id,'PATCH',{draft:false});
console.log('Published '+published.html_url);
