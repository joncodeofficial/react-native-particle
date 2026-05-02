package com.margelo.nitro.particle

import android.content.Context
import android.graphics.Canvas
import android.graphics.Color
import android.graphics.Paint
import android.graphics.PorterDuff
import android.graphics.PorterDuffXfermode
import android.view.Choreographer
import android.view.View
import androidx.annotation.Keep
import com.facebook.proguard.annotations.DoNotStrip
import java.nio.ByteBuffer
import java.nio.ByteOrder
import java.nio.FloatBuffer

@Keep
@DoNotStrip
class HybridParticleCanvasView(context: Context) : HybridParticleCanvasViewSpec() {
  // React Native passes logical coordinates; Canvas draws in physical px, so we convert at the edge.
  private val density = context.resources.displayMetrics.density

  // ─── Inner drawing View ─────────────────────────────────────────────────────

  private inner class DrawView(ctx: Context) : View(ctx) {
    override fun onDraw(canvas: Canvas) = this@HybridParticleCanvasView.drawFrame(canvas)
    override fun onSizeChanged(w: Int, h: Int, oldW: Int, oldH: Int) {
      if (w > 0 && h > 0) setupEngine(w.toFloat(), h.toFloat())
    }
  }

  private val drawView = DrawView(context)
  override val view: View get() = drawView

  // ─── Props ──────────────────────────────────────────────────────────────────

  override var preset: String = ""
    // The preset payload is also used to derive render-only hints such as shape/blend mode.
    set(value) { field = value; parseShape(); parseBlendMode() }
  override var count: Double = 200.0
  override var emitterX: Double = 0.0
  override var emitterY: Double = 0.0
  override var loop: Boolean = false
  override var emitInterval: Double = 200.0

  // ─── C++ engine ─────────────────────────────────────────────────────────────

  // Native side owns the engine pointer and exposes just enough state to the draw loop.
  private val enginePtr: Long = nativeCreate()
  private var floatBuffer: FloatBuffer? = null
  private val paint = Paint(Paint.ANTI_ALIAS_FLAG)
  private var initialized = false
  private var drawShape: String = "circle"
  private var additiveBlend = false

  private fun parseShape() {
    drawShape = when {
      preset.contains("\"shape\":\"rect\"") -> "rect"
      preset.contains("\"shape\":\"line\"") -> "line"
      else -> "circle"
    }
  }

  private fun parseBlendMode() {
    additiveBlend = preset.contains("\"blendMode\":\"additive\"")
  }

  // ─── Choreographer loop ─────────────────────────────────────────────────────

  private var running = false
  private var lastFrameNanos = 0L
  private var emitTimerNanos = 0L

  private val frameCallback: Choreographer.FrameCallback = Choreographer.FrameCallback { frameNanos ->
    if (!running) return@FrameCallback

    // First frame falls back to ~60 FPS so the engine always advances from a sensible dt.
    val dt = if (lastFrameNanos == 0L) 0.016
             else (frameNanos - lastFrameNanos) / 1_000_000_000.0
    lastFrameNanos = frameNanos

    if (loop) {
      // Looping emitters periodically inject new particles without reallocating the native view.
      val elapsedMs = (frameNanos - emitTimerNanos) / 1_000_000L
      if (elapsedMs >= emitInterval.toLong()) {
        nativeEmit(enginePtr, emitterX.toFloat(), emitterY.toFloat(), count.toInt(), preset)
        emitTimerNanos = frameNanos
      }
    }

    // The C++ step also refreshes the packed buffer consumed below by drawFrame().
    nativeStep(enginePtr, dt)
    drawView.invalidate()
    Choreographer.getInstance().postFrameCallback(frameCallback)
  }

  // ─── Setup ──────────────────────────────────────────────────────────────────

  private fun setupEngine(w: Float, h: Float) {
    // Simulate in logical units so JS coordinates match across Android density buckets.
    val logicalWidth = w / density
    val logicalHeight = h / density
    nativeInitialize(enginePtr, MAX_PARTICLES, logicalWidth, logicalHeight)

    // The renderer reads C++ memory directly; no JS arrays or per-frame copies are involved.
    val buf = nativeGetBuffer(enginePtr)
    buf.order(ByteOrder.nativeOrder())
    floatBuffer = buf.asFloatBuffer()

    // `0` means "center me" for fullscreen effects.
    val cx = if (emitterX == 0.0) logicalWidth / 2f else emitterX.toFloat()
    val cy = if (emitterY == 0.0) logicalHeight / 2f else emitterY.toFloat()
    nativeEmit(enginePtr, cx, cy, count.toInt(), preset)
    nativePlay(enginePtr)
    parseShape()
    parseBlendMode()

    initialized = true
    start()
  }

  private fun start() {
    if (running) return
    // Choreographer keeps simulation and draw cadence aligned with the display refresh rate.
    running = true
    lastFrameNanos = 0L
    emitTimerNanos = 0L
    Choreographer.getInstance().postFrameCallback(frameCallback)
  }

  private fun stop() {
    running = false
    Choreographer.getInstance().removeFrameCallback(frameCallback)
  }

  // ─── Drawing ────────────────────────────────────────────────────────────────

  private fun drawFrame(canvas: Canvas) {
    val fb = floatBuffer ?: return
    val particleCount = nativeGetCount(enginePtr)
    fb.rewind()
    // Blend mode is shared per emitter, so we set it once per frame rather than per particle.
    paint.xfermode = if (additiveBlend) PorterDuffXfermode(PorterDuff.Mode.ADD) else null

    repeat(particleCount) { i ->
      // Packed layout matches ParticleEngineCore::_particleData exactly.
      val o        = i * 8
      val x        = fb[o] * density
      val y        = fb[o + 1] * density
      val size     = fb[o + 2] * density
      val r        = (fb[o + 3] * 255f).toInt()
      val g        = (fb[o + 4] * 255f).toInt()
      val b        = (fb[o + 5] * 255f).toInt()
      val a        = (fb[o + 6] * 255f).toInt()
      val rotation = fb[o + 7]
      paint.color  = Color.argb(a, r, g, b)
      when (drawShape) {
        "rect" -> {
          // Non-circular shapes rotate around the particle center using the native rotation value.
          canvas.save()
          canvas.translate(x, y)
          canvas.rotate(rotation * (180f / Math.PI.toFloat()))
          val half = size / 2f
          canvas.drawRect(-half, -half, half, half, paint)
          canvas.restore()
        }
        "line" -> {
          canvas.save()
          canvas.translate(x, y)
          canvas.rotate(rotation * (180f / Math.PI.toFloat()))
          val w = size * 3f
          val h = size * 0.4f
          canvas.drawRect(-w / 2f, -h / 2f, w / 2f, h / 2f, paint)
          canvas.restore()
        }
        else -> canvas.drawCircle(x, y, size / 2f, paint)
      }
    }
  }

  // ─── Lifecycle ──────────────────────────────────────────────────────────────

  override fun onDropView() {
    // The host view lifecycle owns the native pointer and frame callback subscription.
    stop()
    nativeDestroy(enginePtr)
  }

  // ─── JNI ────────────────────────────────────────────────────────────────────

  private external fun nativeCreate(): Long
  private external fun nativeDestroy(ptr: Long)
  private external fun nativeInitialize(ptr: Long, maxParticles: Int, w: Float, h: Float)
  private external fun nativeEmit(ptr: Long, x: Float, y: Float, count: Int, preset: String)
  private external fun nativeStep(ptr: Long, dt: Double)
  private external fun nativePlay(ptr: Long)
  private external fun nativeReset(ptr: Long)
  private external fun nativeGetCount(ptr: Long): Int
  private external fun nativeGetBuffer(ptr: Long): ByteBuffer

  companion object {
    private const val MAX_PARTICLES = 2000
  }
}
