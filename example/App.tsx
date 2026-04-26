import React, { useState } from 'react';
import {
  SafeAreaView,
  StyleSheet,
  Text,
  TouchableOpacity,
  View,
} from 'react-native';
import { ParticleSystem, ViewAdapter, type PresetName } from 'react-native-particle';

const PRESETS: { label: string; preset: PresetName }[] = [
  { label: '🎉 Confetti', preset: 'confetti' },
  { label: '🔥 Fire', preset: 'fire' },
  { label: '💥 Explosion', preset: 'explosion' },
];

export default function App() {
  const [preset, setPreset] = useState<PresetName>('confetti');
  const [key, setKey] = useState(0);

  const trigger = (p: PresetName) => {
    setPreset(p);
    setKey(k => k + 1);
  };

  return (
    <View style={styles.container}>
      <ParticleSystem
        key={key}
        preset={preset}
        count={300}
        adapter={ViewAdapter}
      />
      <SafeAreaView style={styles.controls}>
        <Text style={styles.title}>react-native-particle</Text>
        <View style={styles.buttons}>
          {PRESETS.map(({ label, preset: p }) => (
            <TouchableOpacity
              key={p}
              style={[styles.btn, preset === p && styles.btnActive]}
              onPress={() => trigger(p)}
            >
              <Text style={styles.btnText}>{label}</Text>
            </TouchableOpacity>
          ))}
        </View>
      </SafeAreaView>
    </View>
  );
}

const styles = StyleSheet.create({
  container: { flex: 1, backgroundColor: '#0d0d0d' },
  controls: {
    position: 'absolute',
    bottom: 0,
    left: 0,
    right: 0,
    paddingBottom: 24,
    alignItems: 'center',
    gap: 16,
  },
  title: { color: '#ffffff88', fontSize: 13, letterSpacing: 1 },
  buttons: { flexDirection: 'row', gap: 10 },
  btn: {
    backgroundColor: 'rgba(255,255,255,0.1)',
    paddingHorizontal: 18,
    paddingVertical: 11,
    borderRadius: 22,
    borderWidth: 1,
    borderColor: 'rgba(255,255,255,0.15)',
  },
  btnActive: {
    backgroundColor: 'rgba(255,255,255,0.22)',
    borderColor: 'rgba(255,255,255,0.4)',
  },
  btnText: { color: '#fff', fontSize: 15, fontWeight: '600' },
});
