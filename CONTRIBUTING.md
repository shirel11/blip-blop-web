# Contributing

Use `npm run build` and `npm run dev` to preview the current browser release. No packages are required for the static build. Browser UI lives in `web/index.html` and `web/public/app.{js,css}`; the C++ engine lives in `port/`.

For UI changes, check English and Hebrew, 320px portrait, iPhone landscape and desktop. Test a real gameplay sequence: menu → character → level → movement/jump/fire → pause → resume. Check simultaneous touch inputs and release them by changing tabs. Avoid adding large libraries or decorative assets to the loading path.

For C++ changes, rebuild the WebAssembly engine, then commit the matching `engine-v2.js` and `game-v2.wasm` together. Restore original data with `python tools/unpack_assets.py` first if necessary. See README for Emscripten setup. Check source/header dependencies; stale objects can corrupt the C++ class layout.

Keep original author notices. Do not apply the browser-layer license to the original game or its assets. Bug reports should include device, OS, browser, orientation and reproduction steps. Never commit API tokens, local SDKs, account credentials or browser profiles.
