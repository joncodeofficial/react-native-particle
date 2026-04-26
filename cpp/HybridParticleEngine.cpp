#include "HybridParticleEngine.hpp"

namespace margelo::nitro::particle
{

  void HybridParticleEngine::initialize(double maxParticles, double width, double height)
  {
    _core.initialize(static_cast<int>(maxParticles), static_cast<float>(width), static_cast<float>(height));
  }

  void HybridParticleEngine::step(double dt) { _core.step(dt); }

  void HybridParticleEngine::emit(double x, double y, double count, const std::string& preset)
  {
    _core.emit(static_cast<float>(x), static_cast<float>(y), static_cast<int>(count), preset);
  }

  void HybridParticleEngine::reset()  { _core.reset(); }
  void HybridParticleEngine::play()   { _core.play(); }
  void HybridParticleEngine::pause()  { _core.pause(); }

  std::shared_ptr<ArrayBuffer> HybridParticleEngine::getParticlesFlat()
  {
    int count = _core.fillParticleData();
    auto* ptr = reinterpret_cast<uint8_t*>(_core.getParticleDataPtr());
    size_t byteSize = static_cast<size_t>(count) * 7 * sizeof(float);
    // Wrap C++ memory — no data copy, no heap alloc. Core outlives this buffer.
    return ArrayBuffer::wrap(ptr, byteSize, []() {});
  }

  double HybridParticleEngine::getParticleCount() { return static_cast<double>(_core.getAliveCount()); }
  bool HybridParticleEngine::getIsRunning()        { return _core.isRunning(); }

} // namespace margelo::nitro::particle
