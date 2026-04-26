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
}

// Suitable for < 100 particles. No extra dependencies.
export function ViewAdapter({ engine, width, height }: AdapterProps) {
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
      const flat = engine.getParticlesFlat()

      const parsed: Particle[] = []
      for (let i = 0; i < flat.length; i += 7) {
        parsed.push({
          x: flat[i]!,
          y: flat[i + 1]!,
          size: flat[i + 2]!,
          r: flat[i + 3]!,
          g: flat[i + 4]!,
          b: flat[i + 5]!,
          a: flat[i + 6]!,
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
      {particles.map((p, i) => (
        <View
          key={i}
          style={{
            position: 'absolute',
            left: p.x - p.size / 2,
            top: p.y - p.size / 2,
            width: p.size,
            height: p.size,
            borderRadius: p.size / 2,
            backgroundColor: `rgba(${Math.round(p.r * 255)},${Math.round(p.g * 255)},${Math.round(p.b * 255)},${p.a})`,
          }}
        />
      ))}
    </View>
  )
}
