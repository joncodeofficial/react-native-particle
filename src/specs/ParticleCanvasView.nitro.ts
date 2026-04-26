import type { HybridView, HybridViewProps } from 'react-native-nitro-modules'

interface ParticleCanvasViewProps extends HybridViewProps {
  preset: string
  count: number
  emitterX: number
  emitterY: number
  loop: boolean
  emitInterval: number
}

export type ParticleCanvasView = HybridView<
  ParticleCanvasViewProps,
  {},
  { ios: 'swift'; android: 'kotlin' }
>
