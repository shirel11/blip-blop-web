from pathlib import Path
import re,json,hashlib
root=Path(__file__).resolve().parents[1]
engine=root/'web/public/engine-v2.js'
source=engine.read_text()
pattern=r"\{\s*['\"]?filename['\"]?:\s*['\"](/data/[^'\"]+)['\"],\s*['\"]?start['\"]?:\s*(\d+),\s*['\"]?end['\"]?:\s*(\d+)\s*\}"
files=[]
for name,start,end in re.findall(pattern,source):
    asset=root/'port'/name.lstrip('/')
    files.append({'name':Path(name).name,'start':int(start),'end':int(end),'sha256':hashlib.sha256(asset.read_bytes()).hexdigest()})
assert len(files)>100,'Could not read the generated package metadata'
manifest={'format':1,'sdk':'6.0.13','uncompressedBytes':files[-1]['end'],'files':files}
(root/'web/public/resources/assets.json').write_text(json.dumps(manifest,indent=2)+'\n')
print('Manifest generated for',len(files),'assets')
