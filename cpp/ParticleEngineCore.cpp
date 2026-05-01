#include "ParticleEngineCore.hpp"

#include <algorithm>
#include <cmath>
#include "json.hpp"

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
    alloc(_rotation); alloc(_spin);
    _active.assign(_maxParticles, 0);

    _aliveIndices.clear();
    _aliveIndices.reserve(_maxParticles);
    _freeSlots.clear();
    _freeSlots.reserve(_maxParticles);
    _particleData.assign(static_cast<size_t>(_maxParticles) * 8, 0.0f);

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

    float spawnX = x, spawnY = y;
    if (p.emitRadius > 0.0f) {
      float angle  = _randRange(0.0f, 6.2831853f);
      float radius = p.emitRadius * std::sqrt(_randRange(0.0f, 1.0f));
      spawnX = x + radius * std::cos(angle);
      spawnY = y + radius * std::sin(angle);
    }

    _x[i] = spawnX;   _y[i] = spawnY;
    _vx[i] = _randRange(p.vxMin, p.vxMax);
    _vy[i] = _randRange(p.vyMin, p.vyMax);
    _ax[i] = p.ax;    _ay[i] = p.ay;
    _drag[i] = p.dampingVelocity;
    _sizeInit[i] = p.sizeStart; _sizeEnd[i] = p.sizeEnd;
    static constexpr float kDegToRad = 3.14159265f / 180.0f;
    _rotation[i] = _randRange(p.rotationMin, p.rotationMax) * kDegToRad;
    _spin[i]     = _randRange(p.spinMin,     p.spinMax)     * kDegToRad;
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
    if (preset.empty() || preset.front() != '{') return;

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
    cfg.dampingVelocity = j.value("dampingVelocity", 1.0f);
    cfg.sizeStart       = j.value("sizeStart",        8.0f);
    cfg.sizeEnd         = j.value("sizeEnd",           0.0f);
    cfg.lifetimeMin     = j.value("lifetimeMin",       0.5f);
    cfg.lifetimeMax     = j.value("lifetimeMax",       1.5f);
    cfg.emitRadius      = j.value("emitRadius",        0.0f);
    cfg.randomColor     = j.value("randomColor",      false);
    cfg.rotationMin     = j.value("rotationMin",       0.0f);
    cfg.rotationMax     = j.value("rotationMax",       0.0f);
    cfg.spinMin         = j.value("spinMin",           0.0f);
    cfg.spinMax         = j.value("spinMax",           0.0f);

    if (j.contains("colorStart") && j["colorStart"].is_array()) {
      cfg.rStart = j["colorStart"][0].get<float>();
      cfg.gStart = j["colorStart"][1].get<float>();
      cfg.bStart = j["colorStart"][2].get<float>();
      cfg.aStart = j["colorStart"][3].get<float>();
    } else { cfg.rStart = 1.0f; cfg.gStart = 1.0f; cfg.bStart = 1.0f; cfg.aStart = 1.0f; }

    if (j.contains("colorEnd") && j["colorEnd"].is_array()) {
      cfg.rEnd = j["colorEnd"][0].get<float>();
      cfg.gEnd = j["colorEnd"][1].get<float>();
      cfg.bEnd = j["colorEnd"][2].get<float>();
      cfg.aEnd = j["colorEnd"][3].get<float>();
    } else { cfg.rEnd = 1.0f; cfg.gEnd = 1.0f; cfg.bEnd = 1.0f; cfg.aEnd = 0.0f; }

    return cfg;
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
      _rotation[i] += _spin[i] * fdt;
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
      dst[7] = _rotation[i];
      dst += 8;
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
