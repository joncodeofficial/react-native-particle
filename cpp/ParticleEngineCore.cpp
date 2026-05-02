#include "ParticleEngineCore.hpp"

#include <algorithm>
#include <cmath>
#include "json.hpp"

namespace margelo::nitro::particle
{

  // Tiny math helpers kept local to the compilation unit because they are used in hot paths.
  static inline float lerp(float a, float b, float t) { return a + (b - a) * t; }
  static inline float clamp01(float v) { return v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v); }
  static inline auto parseCurveEase(const std::string& curve)
  {
    using CurveEase = PresetConfig::CurveEase;
    if (curve == "easeIn") return CurveEase::EaseIn;
    if (curve == "easeOut") return CurveEase::EaseOut;
    if (curve == "pulse") return CurveEase::Pulse;
    return CurveEase::Linear;
  }
  static inline float applyCurveEase(float t, PresetConfig::CurveEase ease)
  {
    // Curves stay intentionally simple/branch-light because they run for every live particle.
    switch (ease) {
    case PresetConfig::CurveEase::EaseIn:
      return t * t;
    case PresetConfig::CurveEase::EaseOut:
      return 1.0f - (1.0f - t) * (1.0f - t);
    case PresetConfig::CurveEase::Pulse:
      return std::sin(t * 3.14159265f);
    case PresetConfig::CurveEase::Linear:
    default:
      return t;
    }
  }
  static inline auto parseBlendMode(const std::string& mode)
  {
    using BlendMode = PresetConfig::BlendMode;
    if (mode == "additive") return BlendMode::Additive;
    return BlendMode::Normal;
  }
  static inline auto parseEmitShape(const std::string& shape)
  {
    using EmitShape = PresetConfig::EmitShape;
    if (shape == "circle") return EmitShape::Circle;
    if (shape == "ring") return EmitShape::Ring;
    if (shape == "line") return EmitShape::Line;
    return EmitShape::Point;
  }

  // ─── Lifecycle ──────────────────────────────────────────────────────────────

  void ParticleEngineCore::initialize(int maxParticles, float width, float height)
  {
    _maxParticles = maxParticles;
    _width = width;
    _height = height;
    _isRunning = false;

    // Every simulation property gets a fixed-size backing array up front.
    auto alloc = [&](auto& v) { v.assign(_maxParticles, {}); };
    alloc(_x);      alloc(_y);
    alloc(_vx);     alloc(_vy);
    alloc(_ax);     alloc(_ay);
    alloc(_turbulenceX); alloc(_turbulenceY);
    alloc(_drag);
    alloc(_sizeInit); alloc(_sizeEnd);
    _sizeEase.assign(_maxParticles, static_cast<uint8_t>(PresetConfig::CurveEase::Linear));
    alloc(_rInit);  alloc(_gInit);  alloc(_bInit);  alloc(_aInit);
    alloc(_rMid);   alloc(_gMid);   alloc(_bMid);   alloc(_aMid);
    alloc(_rEnd);   alloc(_gEnd);   alloc(_bEnd);   alloc(_aEnd);
    _useAlphaTrack.assign(_maxParticles, 0);
    alloc(_alphaStart); alloc(_alphaEnd);
    _alphaEase.assign(_maxParticles, static_cast<uint8_t>(PresetConfig::CurveEase::Linear));
    alloc(_colorMidPoint);
    alloc(_age);    alloc(_lifetime);
    alloc(_rotation); alloc(_spin);
    _active.assign(_maxParticles, 0);

    _aliveIndices.clear();
    _aliveIndices.reserve(_maxParticles);
    _freeSlots.clear();
    _freeSlots.reserve(_maxParticles);
    _particleData.assign(static_cast<size_t>(_maxParticles) * 8, 0.0f);

    // Free slots are pre-seeded so spawning can pop from the back without allocation.
    for (int i = _maxParticles - 1; i >= 0; --i)
      _freeSlots.push_back(i);

    _rng.seed(std::random_device{}());
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

    // Expand the emitter into a concrete spawn position before assigning velocity/state.
    float spawnX = x, spawnY = y;
    switch (p.emitShape) {
    case PresetConfig::EmitShape::Circle: {
      if (p.emitRadius > 0.0f) {
        float angle  = _randRange(0.0f, 6.2831853f);
        float radius = p.emitRadius * std::sqrt(_randRange(0.0f, 1.0f));
        spawnX = x + radius * std::cos(angle);
        spawnY = y + radius * std::sin(angle);
      }
      break;
    }
    case PresetConfig::EmitShape::Ring: {
      if (p.emitRadius > 0.0f) {
        float angle = _randRange(0.0f, 6.2831853f);
        spawnX = x + p.emitRadius * std::cos(angle);
        spawnY = y + p.emitRadius * std::sin(angle);
      }
      break;
    }
    case PresetConfig::EmitShape::Line: {
      float halfWidth = p.emitWidth * 0.5f;
      float halfHeight = p.emitHeight * 0.5f;
      spawnX = x + _randRange(-halfWidth, halfWidth);
      spawnY = y + _randRange(-halfHeight, halfHeight);
      break;
    }
    case PresetConfig::EmitShape::Point:
    default:
      break;
    }

    // Copy preset data into the slot so the step/fill loops can stay data-oriented.
    _x[i] = spawnX;   _y[i] = spawnY;
    _vx[i] = _randRange(p.vxMin, p.vxMax);
    _vy[i] = _randRange(p.vyMin, p.vyMax);
    _ax[i] = p.ax;    _ay[i] = p.ay;
    _turbulenceX[i] = p.turbulenceX;
    _turbulenceY[i] = p.turbulenceY;
    _drag[i] = p.dampingVelocity;
    _sizeInit[i] = p.sizeStart; _sizeEnd[i] = p.sizeEnd;
    _sizeEase[i] = static_cast<uint8_t>(p.sizeEase);
    static constexpr float kDegToRad = 3.14159265f / 180.0f;
    _rotation[i] = _randRange(p.rotationMin, p.rotationMax) * kDegToRad;
    _spin[i]     = _randRange(p.spinMin,     p.spinMax)     * kDegToRad;
    _age[i] = 0.0f;
    _lifetime[i] = _randRange(p.lifetimeMin, p.lifetimeMax);

    if (p.randomColor)
    {
      // Fast HSV-ish hue sampling to avoid storing a separate palette per particle.
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
      _colorMidPoint[i] = -1.0f;
    }
    else
    {
      _rInit[i]=p.rStart; _gInit[i]=p.gStart; _bInit[i]=p.bStart; _aInit[i]=p.aStart;
      _rMid[i] =p.rMid;   _gMid[i] =p.gMid;   _bMid[i] =p.bMid;   _aMid[i] =p.aMid;
      _rEnd[i] =p.rEnd;   _gEnd[i] =p.gEnd;   _bEnd[i] =p.bEnd;   _aEnd[i] =p.aEnd;
      _colorMidPoint[i] = p.colorMidPoint;
    }
    _useAlphaTrack[i] = p.useAlphaTrack ? 1 : 0;
    _alphaStart[i] = p.alphaStart;
    _alphaEnd[i] = p.alphaEnd;
    _alphaEase[i] = static_cast<uint8_t>(p.alphaEase);
  }

  // ─── Public API ─────────────────────────────────────────────────────────────

  void ParticleEngineCore::emit(float x, float y, int count, const std::string& preset)
  {
    if (preset.empty() || preset.front() != '{') return;

    // Re-parse only when the JSON payload changes; loop emitters usually reuse the same preset.
    if (preset != _lastCustomPresetJson) {
      _cachedCustomPreset   = _parsePresetJson(preset);
      _lastCustomPresetJson = preset;
    }

    for (int i = 0; i < count; i++)
      _spawnParticle(x, y, _cachedCustomPreset);
  }

  PresetConfig ParticleEngineCore::_parsePresetJson(const std::string& jsonStr)
  {
    using J = nlohmann::json;
    auto j = J::parse(jsonStr, nullptr, false);

    // Defaults are centralized here so JS can send sparse preset objects.
    PresetConfig cfg{};
    if (j.contains("velocityX") && j["velocityX"].is_array()) {
      cfg.vxMin = j["velocityX"][0].get<float>();
      cfg.vxMax = j["velocityX"][1].get<float>();
    }
    if (j.contains("velocityY") && j["velocityY"].is_array()) {
      cfg.vyMin = j["velocityY"][0].get<float>();
      cfg.vyMax = j["velocityY"][1].get<float>();
    }
    cfg.ax              = j.value("accelerationX",   0.0f);
    cfg.ay              = j.value("accelerationY",   0.0f);
    cfg.turbulenceX     = j.value("turbulenceX",     0.0f);
    cfg.turbulenceY     = j.value("turbulenceY",     0.0f);
    cfg.dampingVelocity = j.value("dampingVelocity", 1.0f);
    cfg.sizeStart       = j.value("sizeStart",        8.0f);
    cfg.sizeEnd         = j.value("sizeEnd",           0.0f);
    cfg.sizeEase        = parseCurveEase(j.value("sizeEase", std::string("linear")));
    cfg.lifetimeMin     = j.value("lifetimeMin",       0.5f);
    cfg.lifetimeMax     = j.value("lifetimeMax",       1.5f);
    cfg.emitRadius      = j.value("emitRadius",        0.0f);
    cfg.emitWidth       = j.value("emitWidth",         0.0f);
    cfg.emitHeight      = j.value("emitHeight",        0.0f);
    cfg.emitShape       = parseEmitShape(
      j.value("emitShape", cfg.emitRadius > 0.0f ? std::string("circle") : std::string("point"))
    );
    cfg.randomColor     = j.value("randomColor",      false);
    cfg.rotationMin     = j.value("rotationMin",       0.0f);
    cfg.rotationMax     = j.value("rotationMax",       0.0f);
    cfg.spinMin         = j.value("spinMin",           0.0f);
    cfg.spinMax         = j.value("spinMax",           0.0f);
    cfg.blendMode       = parseBlendMode(j.value("blendMode", std::string("normal")));

    if (j.contains("colorStart") && j["colorStart"].is_array()) {
      cfg.rStart = j["colorStart"][0].get<float>();
      cfg.gStart = j["colorStart"][1].get<float>();
      cfg.bStart = j["colorStart"][2].get<float>();
      cfg.aStart = j["colorStart"][3].get<float>();
    } else { cfg.rStart = 1.0f; cfg.gStart = 1.0f; cfg.bStart = 1.0f; cfg.aStart = 1.0f; }

    cfg.colorMidPoint = -1.0f;
    if (j.contains("colorMid") && j["colorMid"].is_array()) {
      cfg.rMid = j["colorMid"][0].get<float>();
      cfg.gMid = j["colorMid"][1].get<float>();
      cfg.bMid = j["colorMid"][2].get<float>();
      cfg.aMid = j["colorMid"][3].get<float>();
      cfg.colorMidPoint = j.value("colorMidPoint", 0.5f);
    }

    if (j.contains("colorEnd") && j["colorEnd"].is_array()) {
      cfg.rEnd = j["colorEnd"][0].get<float>();
      cfg.gEnd = j["colorEnd"][1].get<float>();
      cfg.bEnd = j["colorEnd"][2].get<float>();
      cfg.aEnd = j["colorEnd"][3].get<float>();
    } else { cfg.rEnd = 1.0f; cfg.gEnd = 1.0f; cfg.bEnd = 1.0f; cfg.aEnd = 0.0f; }

    cfg.useAlphaTrack = j.contains("alphaStart") || j.contains("alphaEnd") || j.contains("alphaEase");
    cfg.alphaStart = j.value("alphaStart", cfg.aStart);
    cfg.alphaEnd = j.value("alphaEnd", cfg.aEnd);
    cfg.alphaEase = parseCurveEase(j.value("alphaEase", std::string("linear")));

    return cfg;
  }

  void ParticleEngineCore::step(double dt)
  {
    if (!_isRunning) return;
    float fdt = static_cast<float>(dt);
    const int alive = static_cast<int>(_aliveIndices.size());

    // Iterate over the dense alive list so dead slots never hit the hot loop.
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
      // Turbulence is sampled per frame, then combined with deterministic acceleration.
      float turbulenceX = _randRange(-_turbulenceX[i], _turbulenceX[i]);
      float turbulenceY = _randRange(-_turbulenceY[i], _turbulenceY[i]);
      _vx[i] = (_vx[i] + (_ax[i] + turbulenceX) * fdt) * _drag[i];
      _vy[i] = (_vy[i] + (_ay[i] + turbulenceY) * fdt) * _drag[i];
      _x[i] += _vx[i] * fdt;
      _y[i] += _vy[i] * fdt;
      _rotation[i] += _spin[i] * fdt;
    }

    // Dead particles are compacted out after the physics step so iteration stays dense.
    _aliveIndices.erase(
      std::remove_if(_aliveIndices.begin(), _aliveIndices.end(),
        [this](int i) { return _active[i] == 0; }),
      _aliveIndices.end());
  }

  int ParticleEngineCore::fillParticleData()
  {
    float* dst = _particleData.data();
    const int alive = static_cast<int>(_aliveIndices.size());

    // The renderer-facing buffer is rebuilt every frame from the SoA simulation state.
    for (int k = 0; k < alive; ++k)
    {
      int i = _aliveIndices[k];
      float t = clamp01(_age[i] / _lifetime[i]);
      float sizeT = applyCurveEase(t, static_cast<PresetConfig::CurveEase>(_sizeEase[i]));
      dst[0] = _x[i];
      dst[1] = _y[i];
      dst[2] = lerp(_sizeInit[i], _sizeEnd[i], sizeT);
      float mp = _colorMidPoint[i];
      if (mp >= 0.0f) {
        // Midpoint gradients are evaluated as two linear segments split at colorMidPoint.
        float t2 = (t < mp)
          ? (mp > 0.0f ? t / mp : 0.0f)
          : (mp < 1.0f ? (t - mp) / (1.0f - mp) : 1.0f);
        if (t < mp) {
          dst[3] = lerp(_rInit[i], _rMid[i], t2);
          dst[4] = lerp(_gInit[i], _gMid[i], t2);
          dst[5] = lerp(_bInit[i], _bMid[i], t2);
          dst[6] = lerp(_aInit[i], _aMid[i], t2);
        } else {
          dst[3] = lerp(_rMid[i], _rEnd[i], t2);
          dst[4] = lerp(_gMid[i], _gEnd[i], t2);
          dst[5] = lerp(_bMid[i], _bEnd[i], t2);
          dst[6] = lerp(_aMid[i], _aEnd[i], t2);
        }
      } else {
        dst[3] = lerp(_rInit[i], _rEnd[i], t);
        dst[4] = lerp(_gInit[i], _gEnd[i], t);
        dst[5] = lerp(_bInit[i], _bEnd[i], t);
        dst[6] = lerp(_aInit[i], _aEnd[i], t);
      }
      if (_useAlphaTrack[i] != 0) {
        // Optional alpha track overrides the alpha coming from the color track.
        float alphaT = applyCurveEase(t, static_cast<PresetConfig::CurveEase>(_alphaEase[i]));
        dst[6] = lerp(_alphaStart[i], _alphaEnd[i], alphaT);
      }
      dst[7] = _rotation[i];
      dst += 8;
    }

    return alive;
  }

  void ParticleEngineCore::reset()
  {
    // Reset clears liveness and ages but keeps all fixed-capacity buffers allocated.
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
