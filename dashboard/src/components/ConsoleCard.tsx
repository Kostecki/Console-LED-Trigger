import {
	Card,
	ColorPicker,
	Group,
	SimpleGrid,
	Stack,
	Text,
} from "@mantine/core";
import { IconBulb, IconBulbOff } from "@tabler/icons-react";
import { useState } from "react";

import type { Board, OnlineStatus } from "../../types/board";

const swatches = [
	"#FF0000",
	"#00FF00",
	"#0000FF",
	"#FFFF00",
	"#FF00FF",
	"#00FFFF",
	"#15b886",
	"#FFFFFF",
];

const ShowLedStatus = (ledsStatus: OnlineStatus) => {
	return ledsStatus === 1 ? (
		<IconBulb size={24} color="#FFB300" />
	) : (
		<IconBulbOff size={24} />
	);
};

export function ConsoleCard({ board }: { board: Board }) {
	const [value, onChange] = useState("rgba(197, 216, 153, 1)");

	return (
		<Card
			withBorder
			shadow="sm"
			radius="md"
			key={board.id}
			mb="lg"
			opacity={board.status === 0 ? 0.5 : 1}
		>
			<Card.Section withBorder inheritPadding py="xs" mb="md">
				<Group justify="space-between">
					<Stack gap={0}>
						<Text fw={500}>{board.name}</Text>
						<Text size="xs" fw={400} c="dimmed" fs="italic">
							{board.id}
						</Text>
					</Stack>
					{ShowLedStatus(board.leds.status)}
				</Group>
			</Card.Section>

			<SimpleGrid cols={2} spacing="md">
				<ColorPicker
					format="rgba"
					value={value}
					onChange={onChange}
					swatchesPerRow={8}
					swatches={swatches}
				/>

				<Stack>
					<Text c="dimmed" size="sm">
						LEDs: {board.leds.status ? "On" : "Off"}
					</Text>
					<Text c="dimmed" size="sm">
						Color: {board.leds.color}
					</Text>
					<Text c="dimmed" size="sm">
						Brightness: {board.leds.brightness}
					</Text>
				</Stack>
			</SimpleGrid>
		</Card>
	);
}
