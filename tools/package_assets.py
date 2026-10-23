from pathlib import Path
import gzip, shutil
import subprocess,sys
p=Path(__file__).resolve().parents[1]/'web/public/resources'
with (p/'game.data').open('rb') as src, gzip.open(p/'game.data.gz','wb',compresslevel=9) as dst:
    shutil.copyfileobj(src,dst)
shutil.copyfile(p/'game.js',p.parent/'engine.js')
shutil.copyfile(p/'game.js',p.parent/'engine-v2.js')
shutil.copyfile(p/'game.wasm',p/'game-v2.wasm')
shutil.copyfile(p.parents[1]/'index.html',p.parent/'index.html')
print([(f.name,f.stat().st_size) for f in p.iterdir()])
subprocess.run([sys.executable,str(Path(__file__).parent/'create_asset_manifest.py')],check=True)
