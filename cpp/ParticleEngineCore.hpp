#pragma once

#include <random>
#include <string>
#include <vector>

namespace margelo::nitro::particle
{

  struct PresetConfig
  {
    enum class EmitShape
    {
      Point = 0,
      Circle = 1,
      Ring = 2,
      Line = 3,
    };
    enum class CurveEase
    {
      Linear = 0,
      EaseIn = 1,
      EaseOut = 2,
      Pulse = 3,
    };

    float vxMin, vxMax;
    float vyMin, vyMax;
    float ax, ay;
    float turbulenceX, turbulenceY;
    float dampingVelocity;
    float sizeStart, sizeEnd;
    float lifetimeMin, lifetimeMax;
    float rStart, gStart, bStart, aStart;
    float rMid,   gMid,   bMid,   aMid;
    float rEnd,   gEnd,   bEnd,   aEnd;
    bool useAlphaTrack;
    float alphaStart, alphaEnd;
    float colorMidPoint; // -1 = no mid color; 0–1 = piecewise breakpoint
    bool randomColor;
    EmitShape emitShape;
    float emitRadius;
    float emitWidth, emitHeight;
    CurveEase sizeEase;
    float rotationMin, rotationMax; // degrees
    float spinMin, spinMax;         // degrees/s
  };

  // Pure C++ physics — no Nitro, no JNI, no JSI.
  // Used by HybridParticleEngine (JS path) and HybridParticleCanvasView (native path).
  class ParticleEngineCore
  {
  public:
    void initialize(int maxParticles, float width, float height);
    void step(double dt);
    void emit(float x, float y, int count, const std::string& preset);
    void reset();
    void play();
    void pause();

    // Write [x,y,size,r,g,b,a] × aliveCount into _particleData. Returns count written.
    int fillParticleData();

    int getAliveCount() const { return static_cast<int>(_aliveIndices.size()); }
    int getMaxParticles() const { return _maxParticles; }
    float* getParticleDataPtr() { return _particleData.data(); }
    bool isRunning() const { return _isRunning; }

  private:
    int _maxParticles = 0;
    float _width = 0.0f, _height = 0.0f;
    bool _isRunning = false;

    std::vector<float> _x, _y;
    std::vector<float> _vx, _vy;
    std::vector<float> _ax, _ay;
    std::vector<float> _turbulenceX, _turbulenceY;
    std::vector<float> _drag;
    std::vector<float> _sizeInit, _sizeEnd;
    std::vector<uint8_t> _sizeEase;
    std::vector<float> _rInit, _gInit, _bInit, _aInit;
    std::vector<float> _rMid,  _gMid,  _bMid,  _aMid;
    std::vector<float> _rEnd,  _gEnd,  _bEnd,  _aEnd;
    std::vector<uint8_t> _useAlphaTrack;
    std::vector<float> _alphaStart, _alphaEnd;
    std::vector<uint8_t> _alphaEase;
    std::vector<float> _colorMidPoint;
    std::vector<float> _age, _lifetime;
    std::vector<float> _rotation, _spin; // radians, radians/s
    std::vector<uint8_t> _active;
    std::vector<int> _aliveIndices;
    std::vector<int> _freeSlots;
    std::vector<float> _particleData; // output: maxParticles * 8 floats [x,y,size,r,g,b,a,rotation]

    std::mt19937 _rng;

    std::string _lastCustomPresetJson;
    PresetConfig _cachedCustomPreset{};

    void _spawnParticle(float x, float y, const PresetConfig& p);
    float _randRange(float lo, float hi);
    PresetConfig _parsePresetJson(const std::string& json);
  };

} // namespace margelo::nitro::particle
