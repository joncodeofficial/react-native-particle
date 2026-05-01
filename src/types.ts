export type PresetName = 'confetti' | 'fire' | 'explosion'

export interface PresetConfig {
  /** Rango de velocidad horizontal [min, max] en px/s. */
  velocityX: [number, number]
  /** Rango de velocidad vertical [min, max] en px/s. */
  velocityY: [number, number]
  /** Aceleración horizontal constante en px/s². Default 0. */
  accelerationX?: number
  /** Aceleración vertical constante en px/s² (gravedad). Default 0. */
  accelerationY?: number
  /**
   * Multiplicador de velocidad por frame (0–1).
   * 1.0 = sin fricción · 0.95 = fricción ligera · 0.90 = fricción fuerte.
   * Default 1.0.
   */
  dampingVelocity?: number
  /** Tamaño de la partícula al nacer en px lógicos. Default 8. */
  sizeStart?: number
  /** Tamaño de la partícula al morir en px lógicos. Default 0. */
  sizeEnd?: number
  /** Vida mínima en segundos. Default 0.5. */
  lifetimeMin?: number
  /** Vida máxima en segundos. Default 1.5. */
  lifetimeMax?: number
  /** Color RGBA al nacer. Componentes 0–1. Default [1,1,1,1]. */
  colorStart?: [number, number, number, number]
  /** Color RGBA al morir. Componentes 0–1. Default [1,1,1,0]. */
  colorEnd?: [number, number, number, number]
  /** Si true, cada partícula elige un hue aleatorio ignorando colorStart/colorEnd. */
  randomColor?: boolean
  /**
   * Radio de emisión en px. Las partículas nacen dentro de un disco
   * de este radio centrado en la posición del emisor. Default 0 (emisor puntual).
   */
  emitRadius?: number
}

export interface EmitterConfig {
  id: number | string
  x: number
  y: number
}
