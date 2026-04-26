import React from 'react';
import { StyleSheet, Text, useWindowDimensions } from 'react-native';
import { NativeParticleSystem } from 'react-native-particle';
import FPSMeter from './FPSMeter';
import Layout from './Layout';

const EMITTER_COUNT = 3;
const PARTICLES_PER_EMITTER = 2000;

export default function App() {
  const { width, height } = useWindowDimensions();

  const emitters = Array.from({ length: EMITTER_COUNT }, (_, i) => ({
    id: i,
    x: (width / (EMITTER_COUNT + 1)) * (i + 1),
    y: height - 160,
  }));

  return (
    <Layout backgroundColor="#0d0d0d" statusBarStyle="light-content" noPadding>
      {emitters.map(({ id, x, y }) => (
        <NativeParticleSystem
          key={id}
          preset="confetti"
          count={PARTICLES_PER_EMITTER}
          x={x}
          y={y}
          loop
          emitInterval={200}
        />
      ))}
      <FPSMeter />
      <Text style={styles.label}>
        🔥 NativeCanvas · {EMITTER_COUNT} emitters ·{' '}
        {PARTICLES_PER_EMITTER * EMITTER_COUNT} particles
      </Text>
    </Layout>
  );
}

const styles = StyleSheet.create({
  label: {
    position: 'absolute',
    bottom: 40,
    alignSelf: 'center',
    color: '#ffffff55',
    fontSize: 13,
    letterSpacing: 1,
  },
});
