import React from 'react';
import { StyleSheet, Text, View, useWindowDimensions } from 'react-native';
import { NativeParticleSystem } from 'react-native-particle';
import type { PresetConfig } from 'react-native-particle';
import FPSMeter from './FPSMeter';
import Layout from './Layout';

// ─── Presets ───────────────────────────���────────────────────────────────────

// Humo — las partículas CRECEN mientras suben (sizeEnd > sizeStart)
const smoke: PresetConfig = {
  velocityX: [-18, 18],
  velocityY: [-55, -20],
  accelerationY: -4,
  dampingVelocity: 0.988,
  sizeStart: 20,
  sizeEnd: 65,
  shape: 'rect',
  lifetimeMin: 3.0,
  lifetimeMax: 5.5,
  colorStart: [0.55, 0.55, 0.55, 0.5],
  colorEnd: [0.72, 0.72, 0.72, 0.0],
  emitRadius: 8,
};

// Brasas — flotan erráticamente, dorado → rojo
const ember: PresetConfig = {
  velocityX: [-90, 90],
  velocityY: [-200, -30],
  accelerationY: 18,
  dampingVelocity: 0.974,
  sizeStart: 5,
  sizeEnd: 0,
  lifetimeMin: 1.8,
  lifetimeMax: 4.0,
  colorStart: [1.0, 0.78, 0.1, 1.0],
  colorEnd: [1.0, 0.18, 0.0, 0.0],
  emitRadius: 28,
};

// Lava — burbujas pesadas que suben poco y caen
const lava: PresetConfig = {
  velocityX: [-22, 22],
  velocityY: [-110, -30],
  accelerationY: 90,
  dampingVelocity: 0.955,
  sizeStart: 26,
  sizeEnd: 0,
  lifetimeMin: 0.9,
  lifetimeMax: 2.2,
  colorStart: [1.0, 0.38, 0.0, 1.0],
  colorEnd: [0.55, 0.0, 0.0, 0.0],
  emitRadius: 12,
};

// Fuego artificial — ráfaga radial multicolor, frena rápido
const firework: PresetConfig = {
  velocityX: [-400, 400],
  velocityY: [-400, 400],
  accelerationY: 160,
  dampingVelocity: 0.93,
  sizeStart: 7,
  sizeEnd: 0,
  lifetimeMin: 0.5,
  lifetimeMax: 1.8,
  randomColor: true,
  emitRadius: 4,
};

// Eléctrico — chispas rápidas y cortísimas, se frenan con drag fuerte
const electric: PresetConfig = {
  velocityX: [-700, 700],
  velocityY: [-700, 700],
  accelerationY: 0,
  dampingVelocity: 0.87,
  sizeStart: 4,
  sizeEnd: 0,
  lifetimeMin: 0.04,
  lifetimeMax: 0.18,
  colorStart: [0.55, 0.92, 1.0, 1.0],
  colorEnd: [1.0, 1.0, 1.0, 0.0],
  emitRadius: 0,
};

// Sparks — cuadrados rotando (rotation + spin activos)
const spark: PresetConfig = {
  velocityX: [-260, 260],
  velocityY: [-260, 260],
  accelerationY: 220,
  dampingVelocity: 0.952,
  sizeStart: 13,
  sizeEnd: 0,
  lifetimeMin: 0.3,
  lifetimeMax: 0.9,
  colorStart: [1.0, 0.88, 0.2, 1.0],
  colorEnd: [1.0, 0.28, 0.0, 0.0],
  rotationMin: -180,
  rotationMax: 180,
  spinMin: -720,
  spinMax: 720,
  emitRadius: 5,
};

// Fuego — llama tricolor: negro → naranja → amarillo (colorMid)
const fire: PresetConfig = {
  velocityX: [-35, 35],
  velocityY: [-180, -60],
  accelerationY: -8,
  dampingVelocity: 0.970,
  sizeStart: 22,
  sizeEnd: 0,
  lifetimeMin: 1.0,
  lifetimeMax: 2.2,
  colorStart: [0.05, 0.02, 0.0, 1.0],
  colorMid: [1.0, 0.35, 0.0, 1.0],
  colorMidPoint: 0.35,
  colorEnd: [1.0, 0.95, 0.2, 0.0],
  emitRadius: 14,
};

// Lluvia de meteoros — diagonal rápida desde arriba
const meteor: PresetConfig = {
  velocityX: [280, 480],
  velocityY: [320, 520],
  accelerationY: 0,
  dampingVelocity: 0.999,
  sizeStart: 4,
  sizeEnd: 0,
  lifetimeMin: 0.25,
  lifetimeMax: 0.6,
  colorStart: [1.0, 0.95, 0.8, 1.0],
  colorEnd: [0.7, 0.5, 1.0, 0.0],
  emitRadius: 1000,
};

// ─── App ────────────────────────────────────────────────────────────────────

export default function App() {
  const { width, height } = useWindowDimensions();

  const col1 = width * 0.28;
  const col2 = width * 0.72;
  const row1 = height * 0.2;
  const row2 = height * 0.52;
  const row3 = height - 130;

  return (
    <Layout backgroundColor="#0a0a0f" statusBarStyle="light-content" noPadding>
      {/* Meteoros — esquina superior izquierda, caen diagonal */}
      <NativeParticleSystem
        preset={meteor}
        count={40}
        x={0}
        y={0}
        loop
        emitInterval={300}
      />

      {/* Eléctrico — arriba izquierda */}
      <NativeParticleSystem
        preset={electric}
        count={60}
        x={col1}
        y={row1}
        loop
        emitInterval={80}
      />

      {/* Fuego artificial — arriba derecha */}
      <NativeParticleSystem
        preset={firework}
        count={100}
        x={col2}
        y={row1}
        loop
        emitInterval={1800}
      />

      {/* Brasas — centro izquierda */}
      <NativeParticleSystem
        preset={ember}
        count={80}
        x={col1}
        y={row2}
        loop
        emitInterval={200}
      />

      {/* Lava — centro derecha */}
      <NativeParticleSystem
        preset={lava}
        count={60}
        x={col2}
        y={row2}
        loop
        emitInterval={250}
      />

      {/* Humo — abajo izquierda */}
      <NativeParticleSystem
        preset={smoke}
        count={12}
        x={col1}
        y={row3}
        loop
        emitInterval={600}
      />

      {/* Fuego — abajo derecha, llama tricolor */}
      <NativeParticleSystem
        preset={fire}
        count={30}
        x={col2}
        y={row3}
        loop
        emitInterval={120}
      />

      <FPSMeter />

      {/* Labels */}
      <View style={[styles.label, { left: col1 - 30, top: row1 + 40 }]}>
        <Text style={styles.labelText}>electric</Text>
      </View>
      <View style={[styles.label, { left: col2 - 32, top: row1 + 40 }]}>
        <Text style={styles.labelText}>firework</Text>
      </View>
      <View style={[styles.label, { left: col1 - 22, top: row2 + 40 }]}>
        <Text style={styles.labelText}>ember</Text>
      </View>
      <View style={[styles.label, { left: col2 - 16, top: row2 + 40 }]}>
        <Text style={styles.labelText}>lava</Text>
      </View>
      <View style={[styles.label, { left: col1 - 22, top: row3 - 50 }]}>
        <Text style={styles.labelText}>smoke</Text>
      </View>
      <View style={[styles.label, { left: col2 - 16, top: row3 - 50 }]}>
        <Text style={styles.labelText}>fire</Text>
      </View>
    </Layout>
  );
}

const styles = StyleSheet.create({
  label: {
    position: 'absolute',
  },
  labelText: {
    color: '#ffffff30',
    fontSize: 11,
    letterSpacing: 1.5,
  },
});
