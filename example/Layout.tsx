import { ReactNode } from 'react';
import { StatusBar, StyleSheet, View } from 'react-native';
import { useSafeAreaInsets } from 'react-native-safe-area-context';

interface LayoutProps {
  children: ReactNode;
  backgroundColor?: string;
  statusBarStyle?: 'default' | 'light-content' | 'dark-content';
  noPadding?: boolean;
  edges?: ('top' | 'bottom' | 'left' | 'right')[];
  hasHeader?: boolean;
}

export default function Layout({
  children,
  backgroundColor = '#fafafa',
  statusBarStyle,
  noPadding = false,
  edges = ['top', 'bottom'],
  hasHeader = false,
}: LayoutProps) {
  const insets = useSafeAreaInsets();
  const finalStatusBarStyle = statusBarStyle || (hasHeader ? 'light-content' : 'default');

  const paddingTop = edges.includes('top') ? insets.top : 0;
  const paddingBottom = edges.includes('bottom') ? insets.bottom : 0;
  const paddingLeft = edges.includes('left') ? insets.left : 0;
  const paddingRight = edges.includes('right') ? insets.right : 0;

  return (
    <View style={[styles.container, { backgroundColor, paddingTop, paddingBottom, paddingLeft, paddingRight }]}>
      <View style={[styles.inner, noPadding && styles.noPadding]}>{children}</View>
      <StatusBar barStyle={finalStatusBarStyle} />
    </View>
  );
}

const styles = StyleSheet.create({
  container: { flex: 1 },
  inner: { flex: 1, paddingHorizontal: 16 },
  noPadding: { paddingHorizontal: 0 },
});
