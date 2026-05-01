import React, { useEffect, useRef, useState } from 'react'
import type { AdapterProps } from './types'

// Lazy-require so the file doesn't crash when Skia is not installed.
// eslint-disable-next-line @typescript-eslint/no-explicit-any
let SkiaCanvas: any, SkiaCircle: any, SkiaRect: any, SkiaGroup: any

try {
  const skia = require('@shopify/react-native-skia')
  SkiaCanvas = skia.Canvas
  SkiaCircle = skia.Circle
  SkiaRect = skia.Rect
  SkiaGroup = skia.Group
} catch {
  // Skia not installed – detectRenderer() will never route here if unavailable.
}

interface Particle {
  x: number
  y: number
  size: number
  color: string
  rotation: number
}

// Suitable for < 10,000 particles with Skia GPU rendering.
// Upgrade path: replace Circle/Rect components with drawAtlas() for ultimate perf.
export function SkiaAdapter({ engine, width, height, shape = 'circle' }: AdapterProps) {
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
      const flat = new Float32Array(engine.getParticlesFlat(), 0, count * 8)

      const parsed: Particle[] = []
      for (let i = 0; i < count; i++) {
        const o = i * 8
        parsed.push({
          x: flat[o]!,
          y: flat[o + 1]!,
          size: flat[o + 2]!,
          color: `rgba(${Math.round((flat[o + 3]!) * 255)},${Math.round((flat[o + 4]!) * 255)},${Math.round((flat[o + 5]!) * 255)},${flat[o + 6]})`,
          rotation: flat[o + 7]!,
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
      {particles.map((p, i) => {
        if (shape === 'circle') {
          return (
            <SkiaCircle key={i} cx={p.x} cy={p.y} r={Math.max(p.size / 2, 0.5)} color={p.color} />
          )
        }
        const w = shape === 'line' ? p.size * 3 : p.size
        const h = shape === 'line' ? p.size * 0.4 : p.size
        return (
          <SkiaGroup
            key={i}
            transform={[{ translateX: p.x }, { translateY: p.y }, { rotate: p.rotation }]}
          >
            <SkiaRect x={-w / 2} y={-h / 2} width={w} height={h} color={p.color} />
          </SkiaGroup>
        )
      })}
    </SkiaCanvas>
  )
}
