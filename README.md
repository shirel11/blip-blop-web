# Blip & Blop Web

[Play in English](https://shirel11.github.io/blip-blop-web/?lang=en) · [לשחק בעברית](https://shirel11.github.io/blip-blop-web/?lang=he) · [תיעוד בעברית](README.he.md)

A community browser edition of the classic arcade adventure. The original game engine runs as WebAssembly; the browser interface supports English and Hebrew, keyboard play and multi-touch controls. No installation or account is required.

## Play

1. Choose **Load game**. The first download is about 53 MB.
2. Choose **Play now**. Press Enter or OK through the original menus, select one/two players and a character.
3. On a phone, landscape provides the most room. Use the directional pad with one thumb and fire/jump with the other. **Game mode** fills the available screen even when iPhone Safari does not offer browser fullscreen.

| Action | Player 1 | Player 2 |
| --- | --- | --- |
| Aim / move | Arrow keys | Q / S / D / F |
| Fire | Z | Tab |
| Jump | X | G |
| Cow bomb | Space | H |
| Confirm | Enter | — |
| Pause | Esc or Pause button | — |

The **browser UI** is bilingual. The original in-game story and menus are English. Two-player mode is local shared-keyboard play, not online multiplayer.

## Run locally

Requires Node.js 20 or newer. The static web build has no npm dependencies.

```sh
git clone https://github.com/shirel11/blip-blop-web.git
cd blip-blop-web
npm run check
npm run build
npm run dev
```

Open http://127.0.0.1:8083. Serve over HTTP; do not open HTML as a local file.

## Project layout

| Location | Purpose |
| --- | --- |
| `web/index.html` | Page, accessibility structure and share metadata |
| `web/public/app.js` | Language, streaming loader, touch input and browser controls |
| `web/public/app.css` | Portrait, landscape, safe-area and desktop layouts |
| `web/public/engine-v2.js` | Compiled Emscripten engine glue |
| `web/public/resources/game-v2.wasm` | Matching WebAssembly engine |
| `web/public/resources/game.data.gz` | Lossless compressed original game data |
| `web/public/resources/assets.json` | Offsets and SHA-256 checksums for restoring source assets |
| `port/` | Browser-adapted C++ source with original notices preserved |
| `scripts/` | Dependency-free static build, preview and release checks |
| `.github/workflows/` | Automatic Pages publishing and manual engine rebuild |

## Build the engine

The checked-in engine JS, WASM and compressed game package are sufficient for web UI development. To change C++ code, install **Emscripten 6.0.13**, activate it, then:

```sh
python tools/unpack_assets.py
python tools/build_port.py
python tools/package_assets.py
python tools/create_asset_manifest.py
npm run check
npm run build
```

Place a local SDK under `emsdk/`, put `em++` on PATH, or set `EMXX` to its executable. The compiler caches objects under `.build/` and invalidates them when source/header files change. Always publish a matching `engine-v2.js` / `game-v2.wasm` pair. Versioned filenames keep an older open page working during a release. Asset offsets must match the generated engine metadata.

The manual **Rebuild WebAssembly engine** Actions workflow also builds with the pinned Emscripten container and uploads a release artifact for review. It does not automatically replace the checked-in release.

## Publish

GitHub Pages is configured through **Settings → Pages → GitHub Actions**. Pushes to `main` validate the release, build `dist/` and deploy automatically. All asset paths are relative, so repository subpaths and custom domains work.

Default public address: **https://shirel11.github.io/blip-blop-web/**.

To use a domain you own, add it in Pages settings and configure DNS as instructed by GitHub. There is no domain included with this repository. Store no service credentials in the website or Git history.

## Mobile performance and limits

This release reuses a streaming frame texture, removes duplicate browser yields, streams decompression without holding multiple compressed copies, starts with a 64 MiB WASM heap and limits simulation catch-up after a backgrounded tab resumes. The heap can grow when required; 64 MiB is not a cap or a claim about total Safari process memory.

The complete game data still expands to about 124 MB, and original graphics are decoded during play. Real-device verification on iPhone 15 Pro / iOS 27 is still necessary; desktop WebKit emulation does not reproduce iOS memory-pressure termination. See [ROADMAP.md](ROADMAP.md) for smaller per-level downloads and the next engine work.

## Contribute and credits

See [CONTRIBUTING.md](CONTRIBUTING.md), [ROADMAP.md](ROADMAP.md) and the device-aware bug-report template. Original game: **Loaded Studio**. This project is an independent community adaptation. Original authorship notices remain in the C++ source; no ownership of the original game or its assets is claimed.

For automated browser checks, run `npm ci --prefix tools`, install the Playwright Chromium and WebKit browsers, then run `node tools/verify_release.cjs` while the local preview is running. `GAME_URL` can point the same checks at a published release.

The [MIT license](LICENSE) covers newly authored browser integration, tooling and documentation only. Original game materials and bundled libraries have separate status; read [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).
