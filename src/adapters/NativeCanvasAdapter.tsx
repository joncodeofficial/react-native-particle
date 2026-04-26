import React from 'react'
import { StyleSheet } from 'react-native'
import { getHostComponent } from 'react-native-nitro-modules'
import viewConfig from '../../nitrogen/generated/shared/json/ParticleCanvasViewConfig.json'
import type { PresetName } from '../ParticleSystem'

// Native canvas view — renders particles via Android Canvas / iOS Core Graphics.
// No React reconciler, no shadow tree updates on the particle loop.
// Choreographer (Android) / CADisplayLink (iOS) drives the loop entirely on the UI thread.
const NativeParticleCanvasView = getHostComponent<
  {
    preset: string
    count: number
    emitterX: number
    emitterY: number
    loop: boolean
    emitInterval: number
  },
  {}
>('ParticleCanvasView', () => viewConfig)

export interface NativeCanvasProps {
  preset: PresetName
  count?: number
  x?: number
  y?: number
  loop?: boolean
  emitInterval?: number
}

export function NativeParticleSystem({
  preset,
  count = 200,
  x = 0,
  y = 0,
  loop = false,
  emitInterval = 200,
}: NativeCanvasProps) {
  return (
    <NativeParticleCanvasView
      preset={preset}
      count={count}
      emitterX={x}
      emitterY={y}
      loop={loop}
      emitInterval={emitInterval}
      style={StyleSheet.absoluteFill}
    />
  )
}
