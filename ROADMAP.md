# Roadmap

## feat

- [ ] **ReanimatedRenderer** — Implement `useFrameCallback` loop on the UI thread instead of rAF on the JS thread. Suitable for < 2,000 particles without Skia.
- [ ] **onComplete callback** — Detect `particleCount === 0` inside the renderer loop and invoke the `onComplete` prop.
- [ ] **loop prop** — Re-emit automatically when `particleCount === 0 && loop === true`.
- [ ] **Imperative ref API** — Expose `play()`, `pause()`, `reset()`, and `emit()` via `useImperativeHandle` so effects can be triggered programmatically.
- [ ] **Custom preset API** — Accept a full `PresetConfig` object as the `preset` prop instead of only named strings.
- [ ] **Preset overrides** — Accept an `overrides` prop to patch individual fields of a built-in preset (e.g. `overrides={{ colorStart: '#00aaff' }}`).
- [ ] **Preset: snow** — Slow drifting flakes with slight horizontal oscillation.
- [ ] **Preset: rain** — Fast vertical drops with narrow horizontal spread.
- [ ] **Preset: smoke** — Large, slow, fading grey plumes drifting upward.
- [ ] **Preset: sparkles** — Short-lived glittering points with random directions.
- [ ] **Preset: bubbles** — Rising soft circles that fade out at the top.
- [ ] **Preset: fireworks** — Upward burst that spawns a secondary radial explosion at peak height.
- [ ] **Renderer count warnings** — Warn in dev mode when `count` exceeds the practical limit for the selected renderer (`> 100` for `view`, `> 2000` for `reanimated`).

---

## refactor

- [ ] **`_findFreeSlot()` → free-list** — Replace the O(n) linear scan with a stack-based free-list so slot lookup is O(1) regardless of `maxParticles`.
- [ ] **Per-renderer game loop abstraction** — Extract the rAF loop + `engine.step(dt)` call into a shared `useParticleLoop` hook consumed by all renderers, eliminating duplicated loop logic.
- [ ] **Flat buffer parser** — Move the `flat[i..i+7]` unpacking into a single shared util (`parseFlat`) used by all renderers instead of duplicating the loop in each file.
- [ ] **`onComplete` wiring** — Connect the `onComplete` prop declared in `ParticleSystemProps` through the component to the renderer loop (currently declared but never called).
- [ ] **Engine re-init on prop change** — Handle `width`/`height` changes (e.g. rotation) by calling `engine.initialize()` again instead of relying on the initial mount dimensions.

---

## performance

- [ ] **Skia `drawAtlas`** — Replace N declarative `Circle` components with a single `canvas.drawAtlas(RSXform[])` call. Drops GPU draw calls from O(n) to 1.
- [ ] **`ArrayBuffer` zero-copy** — Replace `getParticlesFlat(): number[]` with `getBuffer(): ArrayBuffer` backed by C++ memory. Eliminates the JS array allocation and copy on every frame.
- [ ] **SIMD ARM NEON** — Vectorize the `step()` integration loop using `arm_neon.h` (`vld1q_f32` / `vmlaq_f32` / `vst1q_f32`). Updates 4 particles per clock cycle.
- [ ] **Thread pool** — Create a fixed `std::thread` pool in `initialize()` and split the `step()` loop into per-core chunks. Never create threads per frame.
- [ ] **GPU Compute (Metal / Vulkan)** — Move the entire simulation to a compute shader. Requires platform-specific shader authoring and a new native layer.
