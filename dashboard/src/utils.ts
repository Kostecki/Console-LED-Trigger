export const brightnessToPercentage = (brightness: number): number => {
  return Math.round((brightness / 255) * 100);
};
