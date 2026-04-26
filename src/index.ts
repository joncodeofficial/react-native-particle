export { ParticleSystem } from './ParticleSystem'
export type { ParticleSystemProps, PresetName } from './ParticleSystem'
export type { AdapterProps } from './adapters/types'
export type { ParticleEngine } from './specs/ParticleEngine.nitro'

// Adapters — also exported from the main entry for local development.
// In production (npm install) prefer the subpath imports for tree-shaking:
//   import { ViewAdapter } from 'react-native-particle/adapters/view'
export { ViewAdapter } from './adapters/ViewAdapter'
export { SkiaAdapter } from './adapters/SkiaAdapter'
export { ReanimatedAdapter } from './adapters/ReanimatedAdapter'
