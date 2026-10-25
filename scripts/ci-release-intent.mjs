// SPDX-License-Identifier: GPL-2.0-or-later
import fs from 'node:fs/promises';
import {execFileSync} from 'node:child_process';
const event=JSON.parse(await fs.readFile(process.env.GITHUB_EVENT_PATH,'utf8'));
const defaultRef='refs/heads/'+event.repository.default_branch;
let publish=false;
if(process.env.GITHUB_REF===defaultRef){
 if(process.env.GITHUB_EVENT_NAME==='workflow_dispatch')publish=event.inputs?.publish==='true';
 if(process.env.GITHUB_EVENT_NAME==='push'){
  const before=event.before,after=process.env.GITHUB_SHA;
  if(!/^[a-f0-9]{40}$/.test(before)||!/^[a-f0-9]{40}$/.test(after))throw Error('Invalid commit identifiers');
  publish=/^0+$/.test(before)?true:execFileSync('git',['diff','--name-only',before,after,'--','version.json'],{encoding:'utf8'}).trim()==='version.json';
 }
}
await fs.appendFile(process.env.GITHUB_OUTPUT,'publish='+publish+'\n');
console.log('Publish requested: '+publish);
