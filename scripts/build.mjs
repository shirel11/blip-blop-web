import { mkdir, cp, rm } from 'node:fs/promises';
await rm('dist', { recursive: true, force: true });
await mkdir('dist', { recursive: true });
await cp('web/public', 'dist', { recursive: true, filter: (path) => !/(?:resources[\\/]game\.(data|js|wasm)$|[\\/]engine\.js$)/.test(path) });
await cp('web/index.html', 'dist/index.html');
await cp('scripts/nojekyll', 'dist/.nojekyll');
console.log('Static release built in dist/');
