import React from 'react';
import {StyleSheet, Text, useWindowDimensions} from 'react-native';
import {NativeParticleSystem} from 'react-native-particle';
import type {PresetConfig} from 'react-native-particle';
import FPSMeter from './FPSMeter';
import Layout from './Layout';

// Nieve — emite desde arriba y cae despacio
const snow: PresetConfig = {
  velocityX: [-30, 30],
  velocityY: [50, 140],
  accelerationY: 15,
  dampingVelocity: 0.992,
  sizeStart: 6,
  sizeEnd: 4,
  lifetimeMin: 4,
  lifetimeMax: 8,
  colorStart: [0.88, 0.94, 1.0, 0.85],
  colorEnd: [0.88, 0.94, 1.0, 0.0],
  emitRadius: 120,
};

// Fuego artificial — ráfaga radial con colores random
const firework: PresetConfig = {
  velocityX: [-380, 380],
  velocityY: [-380, 380],
  accelerationY: 150,
  dampingVelocity: 0.935,
  sizeStart: 7,
  sizeEnd: 0,
  lifetimeMin: 0.5,
  lifetimeMax: 1.6,
  randomColor: true,
  emitRadius: 3,
};

// Fuego más realista — base ancha, naranja → rojo oscuro
const fire: PresetConfig = {
  velocityX: [-50, 50],
  velocityY: [-320, -70],
  accelerationY: 30,
  dampingVelocity: 0.968,
  sizeStart: 22,
  sizeEnd: 2,
  lifetimeMin: 0.4,
  lifetimeMax: 1.2,
  colorStart: [1.0, 0.55, 0.05, 1.0],
  colorEnd: [0.9, 0.08, 0.0, 0.0],
  emitRadius: 18,
};

export default function App() {
  const {width, height} = useWindowDimensions();

  return (
    <Layout backgroundColor="#0d0d0d" statusBarStyle="light-content" noPadding>
      {/* Nieve cae desde el tope */}
      <NativeParticleSystem
        preset={snow}
        count={150}
        x={width / 2}
        y={0}
        loop
        emitInterval={500}
      />

      {/* Fuego artificial en el centro */}
      <NativeParticleSystem
        preset={firework}
        count={120}
        x={width / 2}
        y={height / 2}
        loop
        emitInterval={1600}
      />

      {/* Fuego en la base */}
      <NativeParticleSystem
        preset={fire}
        count={250}
        x={width / 2}
        y={height - 120}
        loop
        emitInterval={150}
      />

      <FPSMeter />
      <Text style={styles.label}>snow · firework · fire</Text>
    </Layout>
  );
}

const styles = StyleSheet.create({
  label: {
    position: 'absolute',
    bottom: 40,
    alignSelf: 'center',
    color: '#ffffff44',
    fontSize: 13,
    letterSpacing: 2,
  },
});
