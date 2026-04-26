#pragma once
#include <vector>
#include "HybridParticleSpec.hpp"

namespace margelo::nitro::particle {
class HybridParticle : public HybridParticleSpec {
    public:
        HybridParticle() : HybridObject(TAG), HybridParticleSpec() {}
       
        double sum(double a, double b) override;
    };
} // namespace margelo::nitro::particle
