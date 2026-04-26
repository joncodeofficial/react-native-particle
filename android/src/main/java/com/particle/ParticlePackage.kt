package com.particle;

import com.facebook.react.BaseReactPackage;
import com.facebook.react.bridge.NativeModule;
import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.module.model.ReactModuleInfoProvider;
import com.facebook.react.uimanager.ViewManager;
import com.margelo.nitro.particle.ParticleOnLoad;
import com.margelo.nitro.particle.views.HybridParticleCanvasViewManager;

public class ParticlePackage : BaseReactPackage() {
  override fun getModule(name: String, reactContext: ReactApplicationContext): NativeModule? = null

  override fun getReactModuleInfoProvider(): ReactModuleInfoProvider = ReactModuleInfoProvider { emptyMap() }

  override fun createViewManagers(reactContext: ReactApplicationContext): List<ViewManager<*, *>> {
    return listOf(HybridParticleCanvasViewManager())
  }

  companion object {
    init {
      ParticleOnLoad.initializeNative()
    }
  }
}
