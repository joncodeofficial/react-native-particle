import UIKit
import NitroModules

class HybridParticleCanvasView: HybridParticleCanvasViewSpec_base, HybridParticleCanvasViewSpec {

  // ─── Props ──────────────────────────────────────────────────────────────────

  var preset: String = "" {
    didSet {
      // The native draw view derives lightweight render hints from the preset JSON.
      drawView.particleShape = parseShape()
      drawView.blendMode = parseBlendMode()
    }
  }
  var count: Double = 200
  var emitterX: Double = 0
  var emitterY: Double = 0
  var loop: Bool = false
  var emitInterval: Double = 200

  // ─── Native view ────────────────────────────────────────────────────────────

  // UIKit host view that paints the renderer-facing particle buffer.
  private let drawView = ParticleDrawView()
  var view: UIView { drawView }

  // ─── C++ engine (via Obj-C++ bridge) ────────────────────────────────────────

  private let engine = ParticleEngineCoreBridge()
  private static let maxParticles: Int32 = 2000

  // ─── Display link ───────────────────────────────────────────────────────────

  private var displayLink: CADisplayLink?
  private var lastTimestamp: CFTimeInterval = 0
  private var emitTimer: CFTimeInterval = 0
  private var initialized = false

  // ─── Setup ──────────────────────────────────────────────────────────────────

  override init() {
    super.init()
    // Engine init is deferred until layout because we need the resolved native size.
    drawView.onSizeChange = { [weak self] size in
      self?.setupEngine(size: size)
    }
  }

  private func parseShape() -> String {
    guard preset.hasPrefix("{") else { return "circle" }
    if preset.contains("\"shape\":\"rect\"") { return "rect" }
    if preset.contains("\"shape\":\"line\"") { return "line" }
    return "circle"
  }

  private func parseBlendMode() -> CGBlendMode {
    guard preset.hasPrefix("{") else { return .normal }
    if preset.contains("\"blendMode\":\"additive\"") { return .plusLighter }
    return .normal
  }

  private func setupEngine(size: CGSize) {
    guard size.width > 0, size.height > 0 else { return }

    // The C++ engine simulates in the same logical coordinate space as UIKit.
    engine.initialize(
      withMaxParticles: Int32(HybridParticleCanvasView.maxParticles),
      width: Float(size.width),
      height: Float(size.height)
    )

    // `0` means "center me" to keep the JS API ergonomic for fullscreen effects.
    let cx = emitterX == 0 ? Float(size.width / 2) : Float(emitterX)
    let cy = emitterY == 0 ? Float(size.height / 2) : Float(emitterY)
    engine.emit(atX: cx, y: cy, count: Int32(count), preset: preset)
    engine.play()

    drawView.engine = engine
    drawView.particleShape = parseShape()
    drawView.blendMode = parseBlendMode()
    initialized = true
    startLoop()
  }

  private func startLoop() {
    guard displayLink == nil else { return }
    // CADisplayLink keeps simulation and redraw in lockstep with the screen refresh rate.
    let link = CADisplayLink(target: self, selector: #selector(tick(_:)))
    link.add(to: .main, forMode: .common)
    displayLink = link
    lastTimestamp = 0
    emitTimer = 0
  }

  private func stopLoop() {
    displayLink?.invalidate()
    displayLink = nil
  }

  @objc private func tick(_ link: CADisplayLink) {
    let now = link.timestamp
    let dt = lastTimestamp == 0 ? 0.016 : now - lastTimestamp
    lastTimestamp = now

    if loop {
      // Looping emitters periodically inject a fresh burst without recreating the engine.
      if (now - emitTimer) * 1000 >= emitInterval {
        engine.emit(
          atX: Float(emitterX == 0 ? Double(drawView.bounds.width / 2) : emitterX),
          y: Float(emitterY == 0 ? Double(drawView.bounds.height / 2) : emitterY),
          count: Int32(count),
          preset: preset
        )
        emitTimer = now
      }
    }

    // Native rendering consumes the packed particle buffer rebuilt by the C++ core.
    engine.step(dt)
    engine.fillParticleData()
    drawView.particleCount = engine.aliveCount()
    drawView.setNeedsDisplay()
  }

  // ─── Lifecycle ──────────────────────────────────────────────────────────────

  deinit {
    stopLoop()
  }
}

// ─── Drawing View ────────────────────────────────────────────────────────────

final class ParticleDrawView: UIView {

  var engine: ParticleEngineCoreBridge?
  var particleCount: Int32 = 0
  var particleShape: String = "circle"
  var blendMode: CGBlendMode = .normal
  // Notifies the host wrapper when UIKit has a valid size for simulation init/re-init.
  var onSizeChange: ((CGSize) -> Void)?

  override init(frame: CGRect) {
    super.init(frame: frame)
    backgroundColor = .clear
    isOpaque = false
  }

  required init?(coder: NSCoder) { fatalError() }

  override func layoutSubviews() {
    super.layoutSubviews()
    if bounds.size != .zero {
      onSizeChange?(bounds.size)
    }
  }

  override func draw(_ rect: CGRect) {
    guard let ctx = UIGraphicsGetCurrentContext(),
          let ptr = engine?.particleDataPtr(),
          particleCount > 0 else { return }

    // All particles in a view share one blend mode because it is driven by the emitter preset.
    ctx.setBlendMode(blendMode)

    for i in 0..<Int(particleCount) {
      // The packed buffer layout mirrors ParticleEngineCore::_particleData.
      let o        = i * 8
      let x        = CGFloat(ptr[o])
      let y        = CGFloat(ptr[o + 1])
      let size     = CGFloat(ptr[o + 2])
      let r        = CGFloat(ptr[o + 3])
      let g        = CGFloat(ptr[o + 4])
      let b        = CGFloat(ptr[o + 5])
      let a        = CGFloat(ptr[o + 6])
      let rotation = CGFloat(ptr[o + 7])

      ctx.setFillColor(red: r, green: g, blue: b, alpha: a)

      switch particleShape {
      case "rect":
        // Rect/line shapes rotate around the particle center using the C++ rotation value.
        ctx.saveGState()
        ctx.translateBy(x: x, y: y)
        ctx.rotate(by: rotation)
        let half = size / 2
        ctx.fill(CGRect(x: -half, y: -half, width: size, height: size))
        ctx.restoreGState()
      case "line":
        ctx.saveGState()
        ctx.translateBy(x: x, y: y)
        ctx.rotate(by: rotation)
        let w = size * 3, h = size * 0.4
        ctx.fill(CGRect(x: -w / 2, y: -h / 2, width: w, height: h))
        ctx.restoreGState()
      default:
        ctx.fillEllipse(in: CGRect(x: x - size / 2, y: y - size / 2, width: size, height: size))
      }
    }
  }
}
