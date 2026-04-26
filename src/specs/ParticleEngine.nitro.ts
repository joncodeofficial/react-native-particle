import type { HybridObject } from 'react-native-nitro-modules'

export interface ParticleEngine extends HybridObject<{ ios: 'c++'; android: 'c++' }> {
  initialize(maxParticles: number, width: number, height: number): void
  step(dt: number): void
  emit(x: number, y: number, count: number, preset: string): void
  reset(): void
  play(): void
  pause(): void
  getParticlesFlat(): number[]

  readonly particleCount: number
  readonly isRunning: boolean
}
