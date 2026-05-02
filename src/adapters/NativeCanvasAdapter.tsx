import React from 'react'
import { StyleSheet } from 'react-native'
import type { StyleProp, ViewStyle } from 'react-native'
import { getHostComponent } from 'react-native-nitro-modules'
import viewConfig from '../../nitrogen/generated/shared/json/ParticleCanvasViewConfig.json'
import type { PresetName, PresetConfig } from '../types'

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
  preset: PresetName | PresetConfig
  count?: number
  x?: number
  y?: number
  loop?: boolean
  emitInterval?: number
  /** Semantic stacking hint. `style.zIndex` overrides this when provided. */
  layer?: 'background' | 'foreground'
  /** Advanced host-view override. Can be used to customize zIndex, opacity or layout. */
  style?: StyleProp<ViewStyle>
}

const layerStyles = StyleSheet.create({
  background: {
    zIndex: 0,
  },
  foreground: {
    zIndex: 1000,
  },
})

export function NativeParticleSystem({
  preset,
  count = 200,
  x = 0,
  y = 0,
  loop = false,
  emitInterval = 200,
  layer = 'background',
  style,
}: NativeCanvasProps) {
  const presetStr = typeof preset === 'string' ? preset : JSON.stringify(preset)
  return (
    <NativeParticleCanvasView
      preset={presetStr}
      count={count}
      emitterX={x}
      emitterY={y}
      loop={loop}
      emitInterval={emitInterval}
      style={[
        StyleSheet.absoluteFill,
        layer === 'foreground' ? layerStyles.foreground : layerStyles.background,
        style,
      ]}
    />
  )
}
