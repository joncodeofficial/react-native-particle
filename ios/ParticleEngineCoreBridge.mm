#import "ParticleEngineCoreBridge.h"
#import "../cpp/ParticleEngineCore.hpp"

@implementation ParticleEngineCoreBridge {
  margelo::nitro::particle::ParticleEngineCore _core;
}

- (void)initializeWithMaxParticles:(int)maxParticles width:(float)w height:(float)h {
  _core.initialize(maxParticles, w, h);
}

- (void)step:(double)dt         { _core.step(dt); }
- (void)fillParticleData        { _core.fillParticleData(); }
- (void)play                    { _core.play(); }
- (void)reset                   { _core.reset(); }
- (int)aliveCount               { return _core.getAliveCount(); }
- (const float *)particleDataPtr { return _core.getParticleDataPtr(); }

- (void)emitAtX:(float)x y:(float)y count:(int)count preset:(NSString *)preset {
  _core.emit(x, y, count, std::string(preset.UTF8String));
}

@end
