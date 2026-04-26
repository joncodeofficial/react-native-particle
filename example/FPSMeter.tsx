import { useEffect, useRef, useState } from 'react';
import { StyleSheet, Text, View } from 'react-native';

export default function FPSMeter() {
  const [fps, setFps] = useState(0);
  const frameCount = useRef(0);
  const lastSecond = useRef(0);

  useEffect(() => {
    let rafId: number;

    const tick = (time: number) => {
      frameCount.current++;
      if (lastSecond.current === 0) lastSecond.current = time;

      if (time - lastSecond.current >= 1000) {
        setFps(frameCount.current);
        frameCount.current = 0;
        lastSecond.current = time;
      }

      rafId = requestAnimationFrame(tick);
    };

    rafId = requestAnimationFrame(tick);
    return () => cancelAnimationFrame(rafId);
  }, []);

  const color = fps >= 50 ? '#00ff88' : fps >= 30 ? '#ffcc00' : '#ff4444';

  return (
    <View style={styles.container}>
      <Text style={styles.label}>ViewAdapter</Text>
      <Text style={[styles.fps, { color }]}>{fps} FPS</Text>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    position: 'absolute',
    top: 60,
    right: 16,
    backgroundColor: 'rgba(0,0,0,0.6)',
    paddingHorizontal: 12,
    paddingVertical: 8,
    borderRadius: 10,
    alignItems: 'flex-end',
    borderWidth: 1,
    borderColor: 'rgba(255,255,255,0.1)',
  },
  label: {
    color: '#ffffff55',
    fontSize: 10,
    letterSpacing: 0.5,
    marginBottom: 2,
  },
  fps: {
    fontSize: 22,
    fontWeight: '700',
    fontVariant: ['tabular-nums'],
  },
});
