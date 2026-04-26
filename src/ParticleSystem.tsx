import React, { useEffect, useState } from 'react'
import { StyleSheet, View, useWindowDimensions } from 'react-native'
import { NitroModules } from 'react-native-nitro-modules'

import type { AdapterProps } from './adapters/types'
import { ViewAdapter } from './adapters/ViewAdapter'
import type { ParticleEngine } from './specs/ParticleEngine.nitro'

export type PresetName = 'confetti' | 'fire' | 'explosion'

export interface ParticleSystemProps {
  preset: PresetName
  count?: number
  /** Emission X — defaults to horizontal center */
  x?: number
  /** Emission Y — defaults to vertical center */
  y?: number
  autoPlay?: boolean
  /** Pass a specific adapter. Defaults to ViewAdapter (zero dependencies). */
  adapter?: React.ComponentType<AdapterProps>
  onComplete?: () => void
}

const MAX_PARTICLES = 2000

export function ParticleSystem({
  preset,
  count = 200,
  x,
  y,
  autoPlay = true,
  adapter: AdapterComponent = ViewAdapter,
}: ParticleSystemProps) {
  const { width, height } = useWindowDimensions()
  const [engine, setEngine] = useState<ParticleEngine | null>(null)

  useEffect(() => {
    const e = NitroModules.createHybridObject<ParticleEngine>('ParticleEngine')
    e.initialize(MAX_PARTICLES, width, height)

    if (autoPlay) {
      e.emit(x ?? width / 2, y ?? height / 2, count, preset)
      e.play()
    }

    setEngine(e)

    return () => {
      e.reset()
    }
    // intentional: only re-create the engine when the component remounts (key change)
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [])

  if (!engine) return null

  return (
    <View style={StyleSheet.absoluteFill} pointerEvents="none">
      <AdapterComponent engine={engine} width={width} height={height} />
    </View>
  )
}
