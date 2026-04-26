#pragma once
#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

// Objective-C++ wrapper around ParticleEngineCore for Swift consumption.
@interface ParticleEngineCoreBridge : NSObject

- (void)initializeWithMaxParticles:(int)maxParticles width:(float)w height:(float)h;
- (void)step:(double)dt;
- (void)fillParticleData;
- (void)emitAtX:(float)x y:(float)y count:(int)count preset:(NSString *)preset;
- (void)play;
- (void)reset;
- (int)aliveCount;
// Raw pointer to the pre-allocated float buffer — valid as long as this object is alive.
- (const float *)particleDataPtr;

@end

NS_ASSUME_NONNULL_END
