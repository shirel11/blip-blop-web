from pathlib import Path
from concurrent.futures import ThreadPoolExecutor, as_completed
import subprocess,sys,os,shutil
root=Path(__file__).resolve().parents[1]
os.chdir(root)
local_compiler=root/'emsdk/upstream/emscripten/em++.py'
compiler_path=os.environ.get('EMXX') or (str(local_compiler) if local_compiler.exists() else shutil.which('em++'))
if not compiler_path:
 raise SystemExit('Install and activate Emscripten 6.0.13 or set EMXX.')
compiler=[sys.executable,compiler_path] if compiler_path.endswith('.py') else [compiler_path]
sources=sorted(Path('port').glob('*.cpp'))+sorted(Path('port/menus').glob('*.cpp'))
out=Path('.build');out.mkdir(exist_ok=True)
headers_mtime=max(p.stat().st_mtime for p in Path('port').rglob('*.h'))
common=['-std=c++14','-O2','-w','-fexceptions','-DPLATFORM_LINUX','-sUSE_SDL=2','-sUSE_SDL_MIXER=2','-sSDL2_MIXER_FORMATS=["mp3","mod"]']
def compile(p):
 obj=out/(str(p).replace('\\','_').replace('/','_')+'.o')
 if not obj.exists() or obj.stat().st_mtime<max(p.stat().st_mtime, headers_mtime):
  r=subprocess.run(compiler+common+['-c',str(p),'-o',str(obj)],capture_output=True,text=True)
  if r.returncode:raise RuntimeError(str(p)+'\n'+r.stderr)
 return obj.as_posix()
objects=[]
with ThreadPoolExecutor(max_workers=6) as pool:
 for n,result in enumerate(as_completed([pool.submit(compile,p) for p in sources]),1):
  objects.append(result.result())
  if n%20==0:print(f'Compiled {n}/{len(sources)}',flush=True)
# Stable link ordering makes CI output reproducible. A smaller initial heap
# reduces Safari's startup peak; Emscripten grows it only when required.
objects.sort()
release_dir=out/'release'
release_dir.mkdir(exist_ok=True)
args=objects+common+['-sASYNCIFY','-sALLOW_MEMORY_GROWTH=1','-sINITIAL_MEMORY=67108864','-sSTACK_SIZE=8388608','-sFORCE_FILESYSTEM=1','-sEXPORTED_RUNTIME_METHODS=["callMain","FS"]','-sINVOKE_RUN=0','--preload-file','port/data@/data','-o',release_dir.as_posix()+'/game.js']
Path('link.rsp').write_text('\n'.join(args),encoding='utf-8')
print('Linking original game...',flush=True)
subprocess.run(compiler+['@link.rsp'],check=True)
for name in ['game.js','game.wasm','game.data']:
 shutil.copy2(release_dir/name,root/'web/public/resources'/name)
shutil.copy2(release_dir/'game.wasm',root/'web/public/resources/game-v2.wasm')
