#include <jni.h>
#include "../../../../cpp/ParticleEngineCore.hpp"

using Core = margelo::nitro::particle::ParticleEngineCore;

extern "C" {

JNIEXPORT jlong JNICALL
Java_com_margelo_nitro_particle_HybridParticleCanvasView_nativeCreate(JNIEnv*, jobject) {
  return reinterpret_cast<jlong>(new Core());
}

JNIEXPORT void JNICALL
Java_com_margelo_nitro_particle_HybridParticleCanvasView_nativeDestroy(JNIEnv*, jobject, jlong ptr) {
  delete reinterpret_cast<Core*>(ptr);
}

JNIEXPORT void JNICALL
Java_com_margelo_nitro_particle_HybridParticleCanvasView_nativeInitialize(
    JNIEnv*, jobject, jlong ptr, jint maxParticles, jfloat w, jfloat h) {
  reinterpret_cast<Core*>(ptr)->initialize(maxParticles, w, h);
}

JNIEXPORT void JNICALL
Java_com_margelo_nitro_particle_HybridParticleCanvasView_nativeEmit(
    JNIEnv* env, jobject, jlong ptr, jfloat x, jfloat y, jint count, jstring preset) {
  const char* p = env->GetStringUTFChars(preset, nullptr);
  reinterpret_cast<Core*>(ptr)->emit(x, y, count, p);
  env->ReleaseStringUTFChars(preset, p);
}

JNIEXPORT void JNICALL
Java_com_margelo_nitro_particle_HybridParticleCanvasView_nativeStep(
    JNIEnv*, jobject, jlong ptr, jdouble dt) {
  auto* core = reinterpret_cast<Core*>(ptr);
  core->step(dt);
  core->fillParticleData(); // write into pre-allocated buffer for onDraw()
}

JNIEXPORT void JNICALL
Java_com_margelo_nitro_particle_HybridParticleCanvasView_nativePlay(JNIEnv*, jobject, jlong ptr) {
  reinterpret_cast<Core*>(ptr)->play();
}

JNIEXPORT void JNICALL
Java_com_margelo_nitro_particle_HybridParticleCanvasView_nativeReset(JNIEnv*, jobject, jlong ptr) {
  reinterpret_cast<Core*>(ptr)->reset();
}

JNIEXPORT jint JNICALL
Java_com_margelo_nitro_particle_HybridParticleCanvasView_nativeGetCount(JNIEnv*, jobject, jlong ptr) {
  return static_cast<jint>(reinterpret_cast<Core*>(ptr)->getAliveCount());
}

// Returns a DirectByteBuffer pointing to C++ memory — zero copy, cached once in Kotlin.
JNIEXPORT jobject JNICALL
Java_com_margelo_nitro_particle_HybridParticleCanvasView_nativeGetBuffer(JNIEnv* env, jobject, jlong ptr) {
  auto* core = reinterpret_cast<Core*>(ptr);
  return env->NewDirectByteBuffer(
    core->getParticleDataPtr(),
    static_cast<jlong>(core->getMaxParticles()) * 8 * sizeof(float)
  );
}

} // extern "C"
