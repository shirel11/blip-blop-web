"""Restore build-time source assets from the checked-in, lossless game package."""
from pathlib import Path
import gzip,hashlib,json
root=Path(__file__).resolve().parents[1]
manifest=json.loads((root/'web/public/resources/assets.json').read_text())
target=(root/'port/data').resolve()
target.mkdir(parents=True,exist_ok=True)
with gzip.open(root/'web/public/resources/game.data.gz','rb') as package:
    for item in manifest['files']:
        destination=(target/item['name']).resolve()
        if destination.parent!=target:
            raise ValueError('Invalid asset path')
        package.seek(item['start'])
        content=package.read(item['end']-item['start'])
        if hashlib.sha256(content).hexdigest()!=item['sha256']:
            raise ValueError('Asset checksum mismatch: '+item['name'])
        destination.write_bytes(content)
print('Restored',len(manifest['files']),'game assets to port/data/')
