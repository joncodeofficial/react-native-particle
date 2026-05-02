export type PresetName = 'confetti' | 'fire' | 'explosion'

export interface PresetConfig {
  // Velocity ranges are sampled once on spawn and then evolved natively by the C++ core.
  /** Rango de velocidad horizontal [min, max] en px/s. */
  velocityX: [number, number]
  /** Rango de velocidad vertical [min, max] en px/s. */
  velocityY: [number, number]
  /** Aceleración horizontal constante en px/s². Default 0. */
  accelerationX?: number
  /** Aceleración vertical constante en px/s² (gravedad). Default 0. */
  accelerationY?: number
  /** Fuerza aleatoria horizontal por frame en px/s². Default 0. */
  turbulenceX?: number
  /** Fuerza aleatoria vertical por frame en px/s². Default 0. */
  turbulenceY?: number
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
  /** Curva temporal para el tamaño. Default 'linear'. */
  sizeEase?: 'linear' | 'easeIn' | 'easeOut' | 'pulse'
  // Lifetime is normalized to 0..1 inside the native core and drives size/color/alpha tracks.
  /** Vida mínima en segundos. Default 0.5. */
  lifetimeMin?: number
  /** Vida máxima en segundos. Default 1.5. */
  lifetimeMax?: number
  /** Color RGBA al nacer. Componentes 0–1. Default [1,1,1,1]. */
  colorStart?: [number, number, number, number]
  /** Color RGBA intermedio. Habilita gradientes tricolores (ej. negro → naranja → amarillo). */
  colorMid?: [number, number, number, number]
  /** Posición normalizada (0–1) del color medio dentro de la vida de la partícula. Default 0.5. */
  colorMidPoint?: number
  /** Color RGBA al morir. Componentes 0–1. Default [1,1,1,0]. */
  colorEnd?: [number, number, number, number]
  /** Alpha independiente al nacer. Si se usa, separa la opacidad del track de color. */
  alphaStart?: number
  /** Alpha independiente al morir. Si se usa, separa la opacidad del track de color. */
  alphaEnd?: number
  /** Curva temporal para la opacidad independiente. Default 'linear'. */
  alphaEase?: 'linear' | 'easeIn' | 'easeOut' | 'pulse'
  /** Si true, cada partícula elige un hue aleatorio ignorando colorStart/colorEnd. */
  randomColor?: boolean
  // Blend mode is applied per emitter by the native renderer, not per-particle in JS.
  /** Hint de blending nativo. Default 'normal'. */
  blendMode?: 'normal' | 'additive'
  // Emission shape controls where particles spawn, separate from the visual particle `shape`.
  /** Forma geométrica del emisor. Default 'point'. */
  emitShape?: 'point' | 'circle' | 'ring' | 'line'
  /**
   * Radio de emisión en px. Se usa para `circle` y `ring`.
   * Si se pasa sin `emitShape`, se mantiene compatibilidad y se interpreta como `circle`.
   */
  emitRadius?: number
  /** Ancho del emisor en px. Se usa principalmente para `line`. */
  emitWidth?: number
  /** Alto del emisor en px. Se usa principalmente para `line`. */
  emitHeight?: number
  /**
   * Rotación inicial en grados [min, max]. Default [0, 0].
   * Cuando rotationMin/Max o spinMin/Max son distintos de 0, las partículas
   * se renderizan como cuadrados en lugar de círculos.
   */
  rotationMin?: number
  rotationMax?: number
  /** Velocidad angular en grados/s [min, max]. Default [0, 0]. */
  spinMin?: number
  spinMax?: number
  /** Forma de cada partícula. Default 'circle'. */
  shape?: 'circle' | 'rect' | 'line'
}

export interface EmitterConfig {
  // Reserved shape for coordinating multiple emitters from JS if a higher-level API is added later.
  id: number | string
  x: number
  y: number
}
