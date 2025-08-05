import csvData from "@shared/colors.csv?raw";

const rgbToHex = (r: number, g: number, b: number): string => {
  const toHex = (num: number): string => {
    const hex = num.toString(16).toUpperCase();
    return hex.length === 1 ? `0${hex}` : hex;
  };

  return `#${toHex(r)}${toHex(g)}${toHex(b)}`;
};

// Import colors from shared CSV file (colors.csv) and turn into hex swatches
const rawSwatches = csvData
  .trim()
  .split("\n")
  .map((line) => line.split(",").map(Number));
export const swatches = rawSwatches.map(([r, g, b]) => rgbToHex(r, g, b));

export const brightnessToPercentage = (brightness: number): number => {
  return Math.round((brightness / 255) * 100);
};

export const colorIndexToHex = (colorIndex: number): string => {
  if (colorIndex < 0 || colorIndex >= swatches.length) {
    return "#000000";
  }

  return swatches[colorIndex];
};
