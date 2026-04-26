import type { HybridObject } from 'react-native-nitro-modules'

export interface Particle extends HybridObject<{ android: 'c++', ios: 'c++' }> {
  sum(num1: number, num2: number): number
}