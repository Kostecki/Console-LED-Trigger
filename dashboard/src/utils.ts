export const rgbToHex = (r: number, g: number, b: number): string => {
  const toHex = (num: number): string => {
    const hex = num.toString(16).toUpperCase();
    return hex.length === 1 ? `0${hex}` : hex;
  };

  return `#${toHex(r)}${toHex(g)}${toHex(b)}`;
};

export const brightnessToPercentage = (brightness: number): number => {
  return Math.round((brightness / 255) * 100);
};
