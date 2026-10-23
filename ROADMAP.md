# Development roadmap

## 0.2 — Mobile browser foundation (this release)

- Hebrew/English browser UI, saved language and shareable language links.
- iPhone landscape layout, safe areas and independent multi-touch controls.
- A native touch-input bridge to avoid WebKit dropping synthetic keyboard events during focus changes.
- Quick taps are queued until sampled; background/blur cancellation clears pending input. Browser help suspends the engine without opening a second native menu.
- Native menus wait for key release only after handling input, preventing new taps from being swallowed while an idle menu waits.
- Streaming asset decompression and an initial WASM heap of 64 MiB instead of 256 MiB.
- A reusable streaming frame texture; no per-frame texture allocation/destruction.
- Suspend background work and cap simulation catch-up after returning to Safari.
- A public source repository, reproducible static build and automatic Pages deployment.

Verification covers the opening level in desktop Chromium and mobile WebKit emulation. A physical iPhone 15 Pro on iOS 27 still needs device verification; emulation does not reproduce iOS memory-pressure termination or thermal throttling.

## 0.3 — Fast loading and continuity

1. Split the 53 MB compressed package into a small boot/menu pack and level packs. Fetch ahead of level transitions, with retry and progress states. Define budgets before implementation: boot download ≤ 10 MB, stable memory on target iPhone, no unbounded asset retention.
2. Replace generic Asyncify sleeps with an explicit browser-driven frame state machine. Preserve game timing and collision behavior; add deterministic record/replay checks before changing simulation.
3. Persist high scores, options and checkpoints in IndexedDB. Specify checkpoint semantics and version migrations.
4. Add a versioned service worker/PWA, with user-controlled downloads and safe cache eviction. Do not cache every level on install.

## 0.4 — Modern play experience

- Remappable keyboard/gamepad/touch controls, button sizing and left-handed layout.
- Gamepad API, audio settings, onboarding and accessible browser menus.
- Proper engine-level localization; the current story and in-game menus are English.
- Optional high-DPI UI, restrained transitions and a modern HUD while retaining the original art style.
- Test the whole campaign and two-player mode, then add reproducible level-transition and input regression suites.

## Later — Larger engine work

Consider GPU sprite rendering, packed texture atlases, level/editor tools and Worker/OffscreenCanvas experiments only after profiling. A new engine is a separate project with migration tests, not a cosmetic reskin. Online multiplayer requires a networking and deterministic-simulation design; it is not part of the current local two-player mode.

## Release checklist

- Real iPhone Safari: portrait, landscape, app switching, low-power mode, loading interruption and a 15-minute play session.
- Desktop: Chrome, Firefox, Safari, keyboard and two-player controls.
- Verify no stuck touches, no hidden action buttons, correct RTL/LTR and useful load-error recovery.
- Check download size, peak process memory, frame time and device temperature. Capture before/after traces on the same device.
- Publish only a matching engine JS/WASM pair and keep a rollback artifact.
