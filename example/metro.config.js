const { getDefaultConfig } = require('@react-native/metro-config');
const path = require('path');
const { withMetroConfig } = require('react-native-monorepo-config');
const root = path.resolve(__dirname, '..');

/**
 * Metro configuration
 * https://facebook.github.io/metro/docs/configuration
 *
 * @type {import('metro-config').MetroConfig}
 */
const config = withMetroConfig(getDefaultConfig(__dirname), {
  root,
  dirname: __dirname,
});

module.exports = config;
