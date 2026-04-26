#pragma once

#include <random>
#include <string>
#include <unordered_map>
#include <vector>

namespace margelo::nitro::particle
{

  struct PresetConfig
  {
    float vxMin, vxMax;
    float vyMin, vyMax;
    float ax, ay;
    float drag;
    float sizeStart, sizeEnd;
    float lifetimeMin, lifetimeMax;
    float rStart, gStart, bStart, aStart;
    float rEnd, gEnd, bEnd, aEnd;
    bool randomColor;
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
    std::vector<float> _drag;
    std::vector<float> _sizeInit, _sizeEnd;
    std::vector<float> _rInit, _gInit, _bInit, _aInit;
    std::vector<float> _rEnd, _gEnd, _bEnd, _aEnd;
    std::vector<float> _age, _lifetime;
    std::vector<uint8_t> _active;
    std::vector<int> _aliveIndices;
    std::vector<int> _freeSlots;
    std::vector<float> _particleData; // output: maxParticles * 7 floats, pre-allocated

    std::unordered_map<std::string, PresetConfig> _presets;
    std::mt19937 _rng;

    void _initPresets();
    void _spawnParticle(float x, float y, const PresetConfig& p);
    float _randRange(float lo, float hi);
  };

} // namespace margelo::nitro::particle
