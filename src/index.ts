import { NitroModules } from 'react-native-nitro-modules'
import type { Particle as ParticleSpec } from './specs/particle.nitro'

export const Particle =
  NitroModules.createHybridObject<ParticleSpec>('Particle')