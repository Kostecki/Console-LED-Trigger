import csvData from "@shared/colors.csv?raw";
import type { Board } from "types/board";

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

export const percentageToBrightness = (percentage: number): number => {
  return Math.round((percentage / 100) * 255);
};

export const displayColor = (board: Board): string => {
  if (board.leds.colorMode === "custom") {
    const outputColor = board.leds.customColor;
    if (!outputColor.startsWith("#")) {
      return `#${outputColor}`;
    }
    return outputColor;
  }

  if (
    board.leds.colorMode === "palette" &&
    board.leds.colorIndex >= 0 &&
    board.leds.colorIndex < swatches.length
  ) {
    const colorIndex = board.leds.colorIndex;
    return swatches[colorIndex];
  }

  // TODO: Maybe do something with "white"?

  return "#000000"; // Default fallback color
};
