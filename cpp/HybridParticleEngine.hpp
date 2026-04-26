#pragma once

#include <memory>
#include <NitroModules/ArrayBuffer.hpp>
#include "ParticleEngineCore.hpp"
#include "../nitrogen/generated/shared/c++/HybridParticleEngineSpec.hpp"

namespace margelo::nitro::particle
{

  class HybridParticleEngine : public HybridParticleEngineSpec
  {
  public:
    HybridParticleEngine() : HybridObject(TAG) {}

    void initialize(double maxParticles, double width, double height) override;
    void step(double dt) override;
    void emit(double x, double y, double count, const std::string& preset) override;
    void reset() override;
    void play() override;
    void pause() override;
    std::shared_ptr<ArrayBuffer> getParticlesFlat() override;
    double getParticleCount() override;
    bool getIsRunning() override;

  private:
    ParticleEngineCore _core;
  };

} // namespace margelo::nitro::particle
