// SPDX-License-Identifier: GPL-2.0-or-later
import fs from 'node:fs/promises';
import path from 'node:path';
import assert from 'node:assert/strict';
import {fileURLToPath} from 'node:url';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const languages=['en-US','zh-CN','ja-JP','ko-KR'];
let baseline;
for(const lang of languages){
 const text=await fs.readFile(path.join(root,'data/locale',lang+'.ini'),'utf8');
 const entries=text.trim().split(/\r?\n/).map(line=>{const match=line.match(/^([A-Za-z][A-Za-z0-9]*)="(.+)"$/);assert(match,'Invalid '+lang+' entry: '+line);return match[1];});
 assert.equal(entries.length,new Set(entries).size,'Duplicate translation keys');
 const keys=entries.sort();if(!baseline)baseline=keys;else assert.deepEqual(keys,baseline,'Missing '+lang+' keys');
}
const source=await fs.readFile(path.join(root,'src/plugin.cpp'),'utf8');
for(const [,key] of source.matchAll(/obs_module_text\("([^"]+)"\)/g))assert(baseline.includes(key),'Untranslated plugin key '+key);
const messages=await fs.readFile(path.join(root,'installer/messages.iss'),'utf8');
let installerBaseline;
for(const lang of ['en','zh','ja','ko']){
 const entries=messages.split(/\r?\n/).filter(s=>s.startsWith(lang+'.')).map(s=>s.slice(lang.length+1).split('=')[0]);
 assert.equal(entries.length,new Set(entries).size,'Duplicate installer keys');
 const keys=entries.sort();if(!installerBaseline)installerBaseline=keys;else assert.deepEqual(keys,installerBaseline,'Missing installer language '+lang);
}
const setup=await fs.readFile(path.join(root,'installer/setup.iss'),'utf8');
for(const [,key] of setup.matchAll(/CustomMessage\('([^']+)'\)/g))assert(installerBaseline.includes(key),'Untranslated installer key '+key);
console.log('PASS four plugin locales and four installer locales; key parity and referenced strings');
