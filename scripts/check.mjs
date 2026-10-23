import { readFile, access } from 'node:fs/promises';
const html = await readFile('web/index.html', 'utf8');
const app = await readFile('web/public/app.js', 'utf8');
for (const file of ['engine-v2.js', 'app.js', 'app.css', 'resources/game-v2.wasm', 'resources/game.data.gz', 'resources/share.png']) await access('web/public/' + file);
if (!html.includes('viewport-fit=cover')) throw Error('Missing iPhone safe-area metadata');
if (!html.includes('og:image')) throw Error('Missing share metadata');
if (!app.includes("pipeThrough(new DecompressionStream('gzip'))")) throw Error('Missing streaming decompression');
if (html.includes('github.com/') && !html.includes('github.com/shirel11/blip-blop-web')) throw Error('Unexpected source repository');
console.log('Release files and metadata verified');
