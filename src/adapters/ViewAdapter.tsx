import React, { useEffect, useRef, useState } from 'react'
import { View } from 'react-native'
import type { AdapterProps } from './types'

interface Particle {
  x: number
  y: number
  size: number
  r: number
  g: number
  b: number
  a: number
  rotation: number
}

// Suitable for < 100 particles. No extra dependencies.
export function ViewAdapter({ engine, width, height, shape = 'circle' }: AdapterProps) {
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
      const buf = engine.getParticlesFlat()
      const flat = new Float32Array(buf, 0, count * 8)

      const parsed: Particle[] = []
      for (let i = 0; i < count; i++) {
        const o = i * 8
        parsed.push({
          x: flat[o]!,
          y: flat[o + 1]!,
          size: flat[o + 2]!,
          r: flat[o + 3]!,
          g: flat[o + 4]!,
          b: flat[o + 5]!,
          a: flat[o + 6]!,
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
    <View
      style={{ position: 'absolute', width, height, overflow: 'hidden' }}
      pointerEvents="none"
    >
      {particles.map((p, i) => {
        const w = shape === 'line' ? p.size * 3 : p.size
        const h = shape === 'line' ? p.size * 0.4 : p.size
        return (
          <View
            key={i}
            style={{
              position: 'absolute',
              left: p.x - w / 2,
              top: p.y - h / 2,
              width: w,
              height: h,
              borderRadius: shape === 'circle' ? p.size / 2 : 0,
              backgroundColor: `rgba(${Math.round(p.r * 255)},${Math.round(p.g * 255)},${Math.round(p.b * 255)},${p.a})`,
              transform: [{ rotate: `${p.rotation}rad` }],
            }}
          />
        )
      })}
    </View>
  )
}
