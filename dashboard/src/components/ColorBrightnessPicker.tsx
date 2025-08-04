import { Box, Button, ColorPicker, Slider, Text } from "@mantine/core";
import { notifications } from "@mantine/notifications";
import csvData from "@shared/colors.csv?raw";
import { useBoardActions } from "hooks/useBoardActions";
import { useState } from "react";
import type { Board } from "../../types/board";
import { brightnessToPercentage, rgbToHex } from "../utils";

// Import colors from shared CSV file (colors.csv) and turn into hex swatches
const rawSwatches = csvData
	.trim()
	.split("\n")
	.map((line) => line.split(",").map(Number));
const swatches = rawSwatches.map(([r, g, b]) => rgbToHex(r, g, b));

export function ColorBrightnessPicker({ board }: { board: Board }) {
	const [color, onChangeColor] = useState(board.leds.color);
	const [brightness, onChangeBrightness] = useState(
		brightnessToPercentage(board.leds.brightness),
	);

	const { setColorSettings } = useBoardActions();

	const submitSettings = () => {
		try {
			setColorSettings(board.id, color, brightness);

			const successMessage = (
				<Text size="sm">
					<Text span>New color settings sent to board: </Text>
					<Text span fw={700}>{` ${board.name} `}</Text>
					<Text span>({board.id}).</Text>
				</Text>
			);

			notifications.show({
				title: "Success",
				message: successMessage,
				color: "green",
				withBorder: true,
				withCloseButton: false,
			});
		} catch (error) {
			notifications.show({
				title: "Error",
				message: "Failed to send color settings to board.",
				color: "red",
				withBorder: true,
				withCloseButton: false,
			});

			console.error("Error updating color settings:", error);
		}
	};

	return (
		<>
			<ColorPicker
				fullWidth
				format="hex"
				value={color}
				onChange={onChangeColor}
				swatchesPerRow={8}
				swatches={swatches}
				style={{
					pointerEvents: board.status === 0 ? "none" : "auto",
				}}
			/>
			<Box my="sm">
				<Text size="sm" c="dimmed">
					Brightness
				</Text>
				<Slider
					color="blue"
					value={brightness}
					onChange={onChangeBrightness}
					marks={[{ value: 50 }]}
					label={(value) => `${value}%`}
					disabled={!board.status}
				/>
			</Box>
			<Button
				fullWidth
				mt="sm"
				variant="default"
				onClick={submitSettings}
				disabled={!board.status}
			>
				Update LED Settings
			</Button>
		</>
	);
}
