#include "ParticleEngineCore.hpp"

#include <algorithm>

namespace margelo::nitro::particle
{

  static inline float lerp(float a, float b, float t) { return a + (b - a) * t; }
  static inline float clamp01(float v) { return v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v); }

  // ─── Lifecycle ──────────────────────────────────────────────────────────────

  void ParticleEngineCore::initialize(int maxParticles, float width, float height)
  {
    _maxParticles = maxParticles;
    _width = width;
    _height = height;
    _isRunning = false;

    auto alloc = [&](auto& v) { v.assign(_maxParticles, {}); };
    alloc(_x);      alloc(_y);
    alloc(_vx);     alloc(_vy);
    alloc(_ax);     alloc(_ay);
    alloc(_drag);
    alloc(_sizeInit); alloc(_sizeEnd);
    alloc(_rInit);  alloc(_gInit);  alloc(_bInit);  alloc(_aInit);
    alloc(_rEnd);   alloc(_gEnd);   alloc(_bEnd);   alloc(_aEnd);
    alloc(_age);    alloc(_lifetime);
    _active.assign(_maxParticles, 0);

    _aliveIndices.clear();
    _aliveIndices.reserve(_maxParticles);
    _freeSlots.clear();
    _freeSlots.reserve(_maxParticles);
    _particleData.assign(static_cast<size_t>(_maxParticles) * 7, 0.0f);

    for (int i = _maxParticles - 1; i >= 0; --i)
      _freeSlots.push_back(i);

    _rng.seed(std::random_device{}());
    _initPresets();
  }

  void ParticleEngineCore::_initPresets()
  {
    _presets["confetti"] = {
        .vxMin = -200.0f, .vxMax = 200.0f,
        .vyMin = -500.0f, .vyMax = -150.0f,
        .ax = 0.0f,       .ay = 380.0f,
        .drag = 0.985f,
        .sizeStart = 10.0f, .sizeEnd = 8.0f,
        .lifetimeMin = 1.5f, .lifetimeMax = 3.2f,
        .rStart = 1, .gStart = 0, .bStart = 0.5f, .aStart = 1.0f,
        .rEnd = 1,   .gEnd = 0,   .bEnd = 0.5f,   .aEnd = 0.0f,
        .randomColor = true,
    };
    _presets["fire"] = {
        .vxMin = -35.0f, .vxMax = 35.0f,
        .vyMin = -190.0f, .vyMax = -60.0f,
        .ax = 0.0f,      .ay = -15.0f,
        .drag = 0.972f,
        .sizeStart = 18.0f, .sizeEnd = 0.0f,
        .lifetimeMin = 0.45f, .lifetimeMax = 1.1f,
        .rStart = 1.0f, .gStart = 0.55f, .bStart = 0.1f, .aStart = 0.9f,
        .rEnd = 0.45f,  .gEnd = 0.0f,   .bEnd = 0.0f,   .aEnd = 0.0f,
        .randomColor = false,
    };
    _presets["explosion"] = {
        .vxMin = -420.0f, .vxMax = 420.0f,
        .vyMin = -420.0f, .vyMax = 420.0f,
        .ax = 0.0f,       .ay = 160.0f,
        .drag = 0.92f,
        .sizeStart = 12.0f, .sizeEnd = 0.0f,
        .lifetimeMin = 0.25f, .lifetimeMax = 0.8f,
        .rStart = 1.0f, .gStart = 0.88f, .bStart = 0.3f, .aStart = 1.0f,
        .rEnd = 0.85f,  .gEnd = 0.2f,   .bEnd = 0.0f,   .aEnd = 0.0f,
        .randomColor = false,
    };
  }

  // ─── Helpers ────────────────────────────────────────────────────────────────

  float ParticleEngineCore::_randRange(float lo, float hi)
  {
    std::uniform_real_distribution<float> d(lo, hi);
    return d(_rng);
  }

  void ParticleEngineCore::_spawnParticle(float x, float y, const PresetConfig& p)
  {
    if (_freeSlots.empty()) return;

    int i = _freeSlots.back();
    _freeSlots.pop_back();

    _active[i] = 1;
    _aliveIndices.push_back(i);

    _x[i] = x;        _y[i] = y;
    _vx[i] = _randRange(p.vxMin, p.vxMax);
    _vy[i] = _randRange(p.vyMin, p.vyMax);
    _ax[i] = p.ax;    _ay[i] = p.ay;
    _drag[i] = p.drag;
    _sizeInit[i] = p.sizeStart; _sizeEnd[i] = p.sizeEnd;
    _age[i] = 0.0f;
    _lifetime[i] = _randRange(p.lifetimeMin, p.lifetimeMax);

    if (p.randomColor)
    {
      float h = _randRange(0.0f, 6.0f);
      int hi6 = static_cast<int>(h);
      float f = h - static_cast<float>(hi6);
      float q = 1.0f - f;
      switch (hi6 % 6)
      {
      case 0: _rInit[i]=1; _gInit[i]=f; _bInit[i]=0; break;
      case 1: _rInit[i]=q; _gInit[i]=1; _bInit[i]=0; break;
      case 2: _rInit[i]=0; _gInit[i]=1; _bInit[i]=f; break;
      case 3: _rInit[i]=0; _gInit[i]=q; _bInit[i]=1; break;
      case 4: _rInit[i]=f; _gInit[i]=0; _bInit[i]=1; break;
      default:_rInit[i]=1; _gInit[i]=0; _bInit[i]=q; break;
      }
      _aInit[i] = p.aStart;
      _rEnd[i] = _rInit[i]; _gEnd[i] = _gInit[i];
      _bEnd[i] = _bInit[i]; _aEnd[i] = 0.0f;
    }
    else
    {
      _rInit[i]=p.rStart; _gInit[i]=p.gStart; _bInit[i]=p.bStart; _aInit[i]=p.aStart;
      _rEnd[i]=p.rEnd;    _gEnd[i]=p.gEnd;    _bEnd[i]=p.bEnd;    _aEnd[i]=p.aEnd;
    }
  }

  // ─── Public API ─────────────────────────────────────────────────────────────

  void ParticleEngineCore::emit(float x, float y, int count, const std::string& preset)
  {
    auto it = _presets.find(preset);
    if (it == _presets.end()) return;
    for (int i = 0; i < count; i++)
      _spawnParticle(x, y, it->second);
  }

  void ParticleEngineCore::step(double dt)
  {
    if (!_isRunning) return;
    float fdt = static_cast<float>(dt);
    const int alive = static_cast<int>(_aliveIndices.size());

    for (int k = 0; k < alive; ++k)
    {
      int i = _aliveIndices[k];
      _age[i] += fdt;
      if (_age[i] >= _lifetime[i])
      {
        _active[i] = 0;
        _freeSlots.push_back(i);
        continue;
      }
      _vx[i] = (_vx[i] + _ax[i] * fdt) * _drag[i];
      _vy[i] = (_vy[i] + _ay[i] * fdt) * _drag[i];
      _x[i] += _vx[i] * fdt;
      _y[i] += _vy[i] * fdt;
    }

    _aliveIndices.erase(
      std::remove_if(_aliveIndices.begin(), _aliveIndices.end(),
        [this](int i) { return _active[i] == 0; }),
      _aliveIndices.end());
  }

  int ParticleEngineCore::fillParticleData()
  {
    float* dst = _particleData.data();
    const int alive = static_cast<int>(_aliveIndices.size());

    for (int k = 0; k < alive; ++k)
    {
      int i = _aliveIndices[k];
      float t = clamp01(_age[i] / _lifetime[i]);
      dst[0] = _x[i];
      dst[1] = _y[i];
      dst[2] = lerp(_sizeInit[i], _sizeEnd[i], t);
      dst[3] = lerp(_rInit[i], _rEnd[i], t);
      dst[4] = lerp(_gInit[i], _gEnd[i], t);
      dst[5] = lerp(_bInit[i], _bEnd[i], t);
      dst[6] = lerp(_aInit[i], _aEnd[i], t);
      dst += 7;
    }

    return alive;
  }

  void ParticleEngineCore::reset()
  {
    std::fill(_active.begin(), _active.end(), 0);
    std::fill(_age.begin(), _age.end(), 0.0f);
    _aliveIndices.clear();
    _freeSlots.clear();
    for (int i = _maxParticles - 1; i >= 0; --i)
      _freeSlots.push_back(i);
    _isRunning = false;
  }

  void ParticleEngineCore::play()  { _isRunning = true; }
  void ParticleEngineCore::pause() { _isRunning = false; }

} // namespace margelo::nitro::particle
