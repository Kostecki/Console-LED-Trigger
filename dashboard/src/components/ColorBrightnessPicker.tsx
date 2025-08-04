import { Box, Button, ColorPicker, Slider, Text } from "@mantine/core";
import { notifications } from "@mantine/notifications";
import csvData from "@shared/colors.csv?raw";
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
	const [loading, setLoading] = useState(false);
	const [color, onChangeColor] = useState(board.leds.color);
	const [brightness, onChangeBrightness] = useState(
		brightnessToPercentage(board.leds.brightness),
	);

	const submitSettings = () => {
		setLoading(true);

		setTimeout(() => {
			setLoading(false);

			console.log("Updating settings for board:", board.id);
			console.log("New color:", color);
			console.log("New brightness:", brightness);

			const message = (
				<Text>
					New configuration sent to board:{" "}
					<Text span fw={700}>{` ${board.name} `}</Text>
					<Text span>({board.id}).</Text>
				</Text>
			);

			notifications.show({
				title: "Update settings",
				message,
				color: "green",
				withBorder: true,
				autoClose: 5000,
			});
			// TODO:MQTT publish logic goes here
		}, 2000);
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
				loading={loading}
			>
				Update Settings
			</Button>
		</>
	);
}
