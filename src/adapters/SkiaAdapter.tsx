import React, { useEffect, useRef, useState } from 'react'
import type { AdapterProps } from './types'

// Lazy-require so the file doesn't crash when Skia is not installed.
// eslint-disable-next-line @typescript-eslint/no-explicit-any
let SkiaCanvas: any, SkiaCircle: any

try {
  const skia = require('@shopify/react-native-skia')
  SkiaCanvas = skia.Canvas
  SkiaCircle = skia.Circle
} catch {
  // Skia not installed – detectRenderer() will never route here if unavailable.
}

interface Particle {
  x: number
  y: number
  r: number
  color: string
}

// Suitable for < 10,000 particles with Skia GPU rendering.
// Upgrade path: replace Circle components with drawAtlas() for ultimate perf.
export function SkiaAdapter({ engine, width, height }: AdapterProps) {
  if (!SkiaCanvas) {
    throw new Error(
      '[react-native-particle] SkiaRenderer requires @shopify/react-native-skia to be installed.'
    )
  }

  const [particles, setParticles] = useState<Particle[]>([])
  const lastTimeRef = useRef(0)

  useEffect(() => {
    let rafId: number

    const tick = (time: number) => {
      const dt = lastTimeRef.current
        ? (time - lastTimeRef.current) / 1000
        : 0.016
      lastTimeRef.current = time

      engine.step(dt)

      const count = engine.particleCount
      const flat = new Float32Array(engine.getParticlesFlat(), 0, count * 7)

      const parsed: Particle[] = []
      for (let i = 0; i < count; i++) {
        const o = i * 7
        parsed.push({
          x: flat[o]!,
          y: flat[o + 1]!,
          r: Math.max((flat[o + 2]!) / 2, 0.5),
          color: `rgba(${Math.round((flat[o + 3]!) * 255)},${Math.round((flat[o + 4]!) * 255)},${Math.round((flat[o + 5]!) * 255)},${flat[o + 6]})`,
        })
      }

      setParticles(parsed)
      rafId = requestAnimationFrame(tick)
    }

    rafId = requestAnimationFrame(tick)
    return () => cancelAnimationFrame(rafId)
  }, [engine])

  return (
    <SkiaCanvas style={{ position: 'absolute', width, height }}>
      {particles.map((p, i) => (
        <SkiaCircle key={i} cx={p.x} cy={p.y} r={p.r} color={p.color} />
      ))}
    </SkiaCanvas>
  )
}
