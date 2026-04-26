#pragma once

#include <random>
#include <string>
#include <unordered_map>
#include <vector>

#include "../nitrogen/generated/shared/c++/HybridParticleEngineSpec.hpp"

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

  class HybridParticleEngine : public HybridParticleEngineSpec
  {
  public:
    HybridParticleEngine() : HybridObject(TAG) {}

    void initialize(double maxParticles, double width, double height) override;
    void step(double dt) override;
    void emit(double x, double y, double count, const std::string &preset) override;
    void reset() override;
    void play() override;
    void pause() override;
    std::vector<double> getParticlesFlat() override;

    double getParticleCount() override;
    bool getIsRunning() override;

  private:
    int _maxParticles = 0;
    float _width = 0.0f, _height = 0.0f;
    bool _isRunning = false;
    int _activeCount = 0;

    // SoA – one index per particle slot
    std::vector<float> _x, _y;
    std::vector<float> _vx, _vy;
    std::vector<float> _ax, _ay;
    std::vector<float> _drag;
    std::vector<float> _sizeInit, _sizeEnd;
    std::vector<float> _rInit, _gInit, _bInit, _aInit;
    std::vector<float> _rEnd, _gEnd, _bEnd, _aEnd;
    std::vector<float> _age, _lifetime;
    std::vector<bool> _active;

    std::unordered_map<std::string, PresetConfig> _presets;
    std::mt19937 _rng;

    void _initPresets();
    void _spawnParticle(float x, float y, const PresetConfig &p);
    float _randRange(float lo, float hi);
    int _findFreeSlot();
  };

}
