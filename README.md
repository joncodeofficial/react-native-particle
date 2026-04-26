# react-native-particle

A high-performance particle engine for React Native, built with [Nitro Modules](https://github.com/mrousavy/nitro). Simulates thousands of particles entirely in C++ — no JS thread involvement on the render path.

[![Version](https://img.shields.io/npm/v/react-native-particle.svg)](https://www.npmjs.com/package/react-native-particle)
[![Downloads](https://img.shields.io/npm/dm/react-native-particle.svg)](https://www.npmjs.com/package/react-native-particle)
[![License](https://img.shields.io/npm/l/react-native-particle.svg)](https://github.com/jonpena/react-native-particle/LICENSE)

## Requirements

- React Native 0.78.0 or higher (Nitro Views require Fabric)
- Node 18.0.0 or higher

## Installation

```bash
npm install react-native-particle react-native-nitro-modules
```

## Renderers

| Renderer | Dependencies | Particles @ 60fps | Notes |
|---|---|---|---|
| `NativeParticleSystem` | none | ~800+ | Android Canvas / iOS Core Graphics. Zero JS on the render path. |
| `SkiaAdapter` | `@shopify/react-native-skia` | 5000+ | GPU-accelerated. Best for heavy effects. |
| `ViewAdapter` | none | < 50 | React Views. Dev/debug only. |

## Usage

### NativeParticleSystem (recommended)

```tsx
import { NativeParticleSystem } from 'react-native-particle'

<NativeParticleSystem
  preset="fire"
  count={400}
  x={200}
  y={600}
  loop
  emitInterval={200}
/>
```

### ParticleSystem with adapter

```tsx
import { ParticleSystem, SkiaAdapter } from 'react-native-particle'

<ParticleSystem
  preset="confetti"
  count={1000}
  adapter={SkiaAdapter}
/>
```

## Presets

`confetti` · `fire` · `explosion`

## Credits

Bootstrapped with [create-nitro-module](https://github.com/patrickkabwe/create-nitro-module).

## Contributing

Pull requests are welcome. For major changes, please open an issue first.
