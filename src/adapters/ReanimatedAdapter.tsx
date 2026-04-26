import React from 'react'
import type { AdapterProps } from './types'
import { ViewAdapter } from './ViewAdapter'

// TODO: implement a proper Reanimated worklet-driven adapter.
// For now, falls back to ViewAdapter so the engine is functional.
export function ReanimatedAdapter(props: AdapterProps) {
  if (__DEV__) {
    console.warn(
      '[react-native-particle] ReanimatedAdapter is not yet implemented. Falling back to ViewAdapter.'
    )
  }
  return <ViewAdapter {...props} />
}
