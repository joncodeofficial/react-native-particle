#pragma once

#include <random>
#include <string>
#include <vector>

namespace margelo::nitro::particle
{

  struct PresetConfig
  {
    // Spawn geometry used when expanding an emitter into initial particle positions.
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
    enum class BlendMode
    {
      Normal = 0,
      Additive = 1,
    };
    // Initial velocity ranges in logical px/s.
    float vxMin, vxMax;
    float vyMin, vyMax;
    // Deterministic acceleration applied every frame.
    float ax, ay;
    // Random force range sampled every frame to break perfectly ballistic motion.
    float turbulenceX, turbulenceY;
    // Velocity multiplier applied after acceleration/turbulence each frame.
    float dampingVelocity;
    // Size track and optional easing applied over normalized lifetime.
    float sizeStart, sizeEnd;
    float lifetimeMin, lifetimeMax;
    // Color track encoded as normalized RGBA values.
    float rStart, gStart, bStart, aStart;
    float rMid,   gMid,   bMid,   aMid;
    float rEnd,   gEnd,   bEnd,   aEnd;
    // Optional alpha override track, kept separate so opacity can evolve independently.
    bool useAlphaTrack;
    float alphaStart, alphaEnd;
    CurveEase alphaEase;
    float colorMidPoint; // -1 = no mid color; 0–1 = piecewise breakpoint
    bool randomColor;
    // Emitter geometry metadata.
    EmitShape emitShape;
    float emitRadius;
    float emitWidth, emitHeight;
    CurveEase sizeEase;
    BlendMode blendMode;
    // Rotation/spin are kept in degrees at the config layer and converted on spawn.
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

    // Struct-of-arrays layout keeps the hot simulation loop cache-friendly.
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
    // Liveness is tracked separately so dead particles can be recycled quickly.
    std::vector<uint8_t> _active;
    std::vector<int> _aliveIndices;
    std::vector<int> _freeSlots;
    std::vector<float> _particleData; // output: maxParticles * 8 floats [x,y,size,r,g,b,a,rotation]

    // Shared RNG used both for spawn-time variation and per-frame turbulence.
    std::mt19937 _rng;

    // Preset JSON is cached because looping emitters often reuse the same payload.
    std::string _lastCustomPresetJson;
    PresetConfig _cachedCustomPreset{};

    // Spawn a single particle into the next free slot.
    void _spawnParticle(float x, float y, const PresetConfig& p);
    // Helper used anywhere a uniform random sample is needed.
    float _randRange(float lo, float hi);
    // Converts the public JSON preset into the compact native config used by the core.
    PresetConfig _parsePresetJson(const std::string& json);
  };

} // namespace margelo::nitro::particle
