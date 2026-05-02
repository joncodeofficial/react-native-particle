import React from 'react';
import { StyleSheet, Text, View, useWindowDimensions } from 'react-native';
import { NativeParticleSystem } from 'react-native-particle';
import type { PresetConfig } from 'react-native-particle';
import type { NativeCanvasProps } from 'react-native-particle';
import FPSMeter from './FPSMeter';
import Layout from './Layout';

const snow: PresetConfig = {
  velocityX: [-22, 22],
  velocityY: [18, 48],
  accelerationY: 4,
  dampingVelocity: 0.998,
  sizeStart: 6,
  sizeEnd: 4,
  lifetimeMin: 5.5,
  lifetimeMax: 8.5,
  colorStart: [0.96, 0.98, 1.0, 0.9],
  colorEnd: [0.96, 0.98, 1.0, 0.0],
  emitRadius: 54,
};

const fire: PresetConfig = {
  velocityX: [-34, 34],
  velocityY: [-180, -70],
  accelerationY: -8,
  dampingVelocity: 0.972,
  sizeStart: 22,
  sizeEnd: 0,
  lifetimeMin: 1.0,
  lifetimeMax: 2.1,
  colorStart: [0.08, 0.03, 0.0, 1.0],
  colorMid: [1.0, 0.36, 0.02, 1.0],
  colorMidPoint: 0.34,
  colorEnd: [1.0, 0.95, 0.28, 0.0],
  emitRadius: 14,
};

const smoke: PresetConfig = {
  velocityX: [-18, 18],
  velocityY: [-55, -18],
  accelerationY: -3,
  dampingVelocity: 0.989,
  sizeStart: 18,
  sizeEnd: 56,
  lifetimeMin: 3.0,
  lifetimeMax: 5.2,
  colorStart: [0.48, 0.5, 0.54, 0.32],
  colorEnd: [0.76, 0.78, 0.82, 0.0],
  emitRadius: 10,
};

const sparkles: PresetConfig = {
  velocityX: [-110, 110],
  velocityY: [-110, 110],
  accelerationY: 40,
  dampingVelocity: 0.92,
  sizeStart: 5,
  sizeEnd: 0,
  lifetimeMin: 0.18,
  lifetimeMax: 0.45,
  randomColor: true,
  emitRadius: 12,
};

const bubbles: PresetConfig = {
  velocityX: [-26, 26],
  velocityY: [-140, -70],
  accelerationY: -6,
  dampingVelocity: 0.993,
  sizeStart: 10,
  sizeEnd: 22,
  lifetimeMin: 1.8,
  lifetimeMax: 3.2,
  colorStart: [0.55, 0.9, 1.0, 0.35],
  colorEnd: [0.8, 0.97, 1.0, 0.0],
  emitRadius: 12,
};

// Approximation for the current engine. A true secondary burst belongs in the core.
const fireworks: PresetConfig = {
  velocityX: [-420, 420],
  velocityY: [-420, 420],
  accelerationY: 160,
  dampingVelocity: 0.93,
  sizeStart: 7,
  sizeEnd: 0,
  lifetimeMin: 0.55,
  lifetimeMax: 1.7,
  randomColor: true,
  emitRadius: 4,
};

type DemoEffect = {
  key: string;
  label: string;
  caption: string;
  preset: PresetConfig;
  count: number;
  emitInterval: number;
  x: number;
  y: number;
  layer?: NativeCanvasProps['layer'];
};

function EffectCard({
  label,
  caption,
  x,
  y,
}: {
  label: string;
  caption: string;
  x: number;
  y: number;
}) {
  return (
    <>
      <View style={[styles.anchor, { left: x - 44, top: y - 44 }]} />
      <View style={[styles.labelBlock, { left: x - 44, top: y - 82 }]}>
        <Text style={styles.labelTitle}>{label}</Text>
        <Text style={styles.labelCaption}>{caption}</Text>
      </View>
    </>
  );
}

export default function App() {
  const { width, height } = useWindowDimensions();

  const left = width * 0.26;
  const right = width * 0.74;
  const top = height * 0.4;
  const middle = height * 0.64;
  const bottom = height * 0.88;

  const effects: DemoEffect[] = [
    {
      key: 'snow',
      label: 'snow',
      caption: 'soft drift',
      preset: snow,
      count: 20,
      emitInterval: 380,
      x: left,
      y: top,
    },
    {
      key: 'fire',
      label: 'fire',
      caption: 'warm tricolor flame',
      preset: fire,
      count: 28,
      emitInterval: 110,
      x: right,
      y: top,
      layer: 'foreground',
    },
    {
      key: 'smoke',
      label: 'smoke',
      caption: 'slow grey plumes',
      preset: smoke,
      count: 14,
      emitInterval: 520,
      x: left,
      y: middle,
    },
    {
      key: 'sparkles',
      label: 'sparkles',
      caption: 'short color pops',
      preset: sparkles,
      count: 48,
      emitInterval: 140,
      x: right,
      y: middle,
    },
    {
      key: 'bubbles',
      label: 'bubbles',
      caption: 'rising soft circles',
      preset: bubbles,
      count: 10,
      emitInterval: 420,
      x: left,
      y: bottom,
    },
    {
      key: 'fireworks',
      label: 'fireworks',
      caption: 'burst approximation',
      preset: fireworks,
      count: 120,
      emitInterval: 1600,
      x: right,
      y: bottom,
    },
  ];

  return (
    <Layout backgroundColor="#07101c" statusBarStyle="light-content" noPadding>
      <View style={styles.header}>
        <Text style={styles.eyebrow}>native renderer demo</Text>
        <Text style={styles.title}>Particle Gallery</Text>
        <Text style={styles.subtitle}>
          Presets aligned on the native canvas.
        </Text>
      </View>

      {effects.map(effect => (
        <React.Fragment key={effect.key}>
          <NativeParticleSystem
            preset={effect.preset}
            count={effect.count}
            x={effect.x}
            y={effect.y}
            layer={effect.layer}
            loop
            emitInterval={effect.emitInterval}
          />
          <EffectCard
            label={effect.label}
            caption={effect.caption}
            x={effect.x}
            y={effect.y}
          />
        </React.Fragment>
      ))}

      <FPSMeter />
    </Layout>
  );
}

const styles = StyleSheet.create({
  header: {
    position: 'absolute',
    top: 28,
    left: 24,
    right: 24,
  },
  eyebrow: {
    color: 'rgba(188, 201, 224, 0.56)',
    fontSize: 10,
    letterSpacing: 1.4,
    textTransform: 'uppercase',
    marginBottom: 6,
  },
  title: {
    color: '#f4f7fb',
    fontSize: 28,
    fontWeight: '600',
    letterSpacing: -0.6,
  },
  subtitle: {
    marginTop: 6,
    color: 'rgba(201, 210, 228, 0.56)',
    fontSize: 13,
    lineHeight: 18,
  },
  anchor: {
    position: 'absolute',
    width: 88,
    height: 88,
    borderRadius: 44,
    borderWidth: 1,
    borderColor: 'rgba(198, 210, 233, 0.12)',
  },
  labelBlock: {
    position: 'absolute',
    width: 110,
  },
  labelTitle: {
    color: '#f6f8fc',
    fontSize: 13,
    fontWeight: '600',
    textTransform: 'lowercase',
  },
  labelCaption: {
    marginTop: 2,
    color: 'rgba(201, 210, 228, 0.52)',
    fontSize: 11,
    lineHeight: 14,
  },
});
