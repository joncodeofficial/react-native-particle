# Roadmap

## feat

- [ ] **onComplete callback** — Detect `particleCount === 0` inside the renderer loop and invoke the `onComplete` prop.
- [ ] **Imperative ref API** — Expose `play()`, `pause()`, `reset()`, and `emit()` via `useImperativeHandle` so effects can be triggered programmatically.
- [x] **Custom preset API** — Accept a full `PresetConfig` object as the `preset` prop instead of only named strings.
- [ ] **Preset overrides** — Accept an `overrides` prop to patch individual fields of a built-in preset (e.g. `overrides={{ colorStart: '#00aaff' }}`).
- [ ] **Preset: snow** — Slow drifting flakes with slight horizontal oscillation.
- [ ] **Preset: rain** — Fast vertical drops with narrow horizontal spread.
- [ ] **Preset: smoke** — Large, slow, fading grey plumes drifting upward.
- [ ] **Preset: sparkles** — Short-lived glittering points with random directions.
- [ ] **Preset: bubbles** — Rising soft circles that fade out at the top.
- [ ] **Preset: fireworks** — Upward burst that spawns a secondary radial explosion at peak height.
- [ ] **Renderer count warnings** — Warn in dev mode when `count` exceeds the practical limit for the selected renderer.

### Particle quality

- [ ] **Per-particle rotation + spin** — Each particle has a `rotation` angle and `spinVelocity` (deg/s). Needed for non-circle shapes (sparks, leaf, debris). Requires passing rotation in the flat buffer and drawing rotated rects/paths in the native renderers.
- [ ] **Color midpoint** — A third `colorMid` + `colorMidPoint` (0–1) field in `PresetConfig` for tri-color gradients. Enables effects like fire (black → orange → yellow) or plasma (blue → white → cyan) without two separate emitters.
- [ ] **Turbulence / noise force** — Per-frame random force (`turbulenceX`, `turbulenceY` in px/s²) applied to each particle. Makes smoke and ember feel organic instead of ballistic. Can be implemented as a per-frame `_randRange(-t, t)` added to velocity before drag.
- [ ] **Size curve (easing)** — Replace the linear lerp between `sizeStart` and `sizeEnd` with a configurable easing (`sizeEase: 'linear' | 'easeIn' | 'easeOut' | 'pulse'`). A `pulse` curve (small → big → small) enables bubbles, breath, and impact rings.
- [ ] **Opacity curve (independent)** — Separate alpha track from color. `alphaStart`, `alphaEnd`, and an `alphaEase` field so color and opacity can evolve at different rates (e.g. color stays constant while alpha pulses).
- [ ] **Blend mode hint** — A `blendMode: 'normal' | 'additive'` field passed to the native renderer. Additive blending makes electric, fire, and magic effects glow instead of occluding each other. Currently all particles are drawn with standard alpha blend.
- [ ] **Emission shape** — `emitShape: 'point' | 'circle' | 'ring' | 'line'` with corresponding dimensions. A `ring` (fixed radius, not filled) produces halo and shockwave effects. A `line` enables waterfall, rain wall, and horizon effects.

---

## refactor

- [ ] **Per-renderer game loop abstraction** — Extract the loop + `engine.step(dt)` call into a shared `useParticleLoop` hook consumed by all JS renderers.
- [ ] **`onComplete` wiring** — Connect the `onComplete` prop through the component to the renderer loop (currently declared but never called).
- [ ] **Engine re-init on prop change** — Handle `width`/`height` changes (e.g. rotation) by calling `engine.initialize()` again instead of relying on the initial mount dimensions.

---

## performance

- [x] **`ArrayBuffer` zero-copy** — `getParticlesFlat()` returns an `ArrayBuffer` backed by C++ memory. No JS array allocation or copy per frame.
- [ ] **SIMD ARM NEON** — Vectorize `fillParticleData()` with `arm_neon.h` (`vld1q_f32` / `vmlaq_f32` / `vst1q_f32`). Processes 4 particles per instruction on ARM.
- [ ] **Thread pool** — Fixed `std::thread` pool created in `initialize()`. Splits `step()` across cores. Never creates threads per frame. Pays off when preset logic gets heavier (physics fields, forces, collisions).
- [x] **O3 + ffast-math** — Applied to both Android (CMake) and iOS (podspec).
- [x] **NativeCanvas renderer** — Android Canvas / iOS Core Graphics. Choreographer/CADisplayLink loop runs entirely on the UI thread. Zero JS, zero React reconciler on the render path.
- [ ] **Skia `drawAtlas`** — Replace N `Circle` components with a single `canvas.drawAtlas(RSXform[])` call. Drops GPU draw calls from O(n) to 1.
- [ ] **GPU Compute (Metal / Vulkan)** — Move the entire simulation to a compute shader. Requires platform-specific shader authoring and a new native layer.
