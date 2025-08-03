import {
	Accordion,
	Box,
	Button,
	Card,
	CardSection,
	ColorPicker,
	Flex,
	Group,
	LoadingOverlay,
	SimpleGrid,
	Slider,
	Stack,
	Text,
} from "@mantine/core";
import {
	IconBrightness,
	IconBulb,
	IconBulbOff,
	IconPalette,
	IconPower,
	IconWifi,
} from "@tabler/icons-react";
import { useState } from "react";

import csvData from "../../../shared/colors.csv?raw";
import type { Board, OnlineStatus } from "../../types/board";
import { brightnessToPercentage, rgbToHex } from "../utils";

// Import colors from shared CSV file and turn into hex swatches
const rawSwatches = csvData
	.trim()
	.split("\n")
	.map((line) => line.split(",").map(Number));
const swatches = rawSwatches.map(([r, g, b]) => rgbToHex(r, g, b));

const showLedStatus = (ledsStatus: OnlineStatus) => {
	return ledsStatus === 1 ? (
		<IconBulb size={24} color="#FFB300" opacity={0.8} />
	) : (
		<IconBulbOff size={24} opacity={0.5} />
	);
};

const AccordionLabel = ({ board }: { board: Board }) => (
	<Group justify="space-between" align="center">
		<Stack gap={0}>
			<Text fw={500}>{board.name}</Text>
			<Text size="xs" fw={400} c="dimmed" fs="italic">
				{board.id}
			</Text>
		</Stack>

		{showLedStatus(board.leds.status)}
	</Group>
);

export function ConsoleCard({ board }: { board: Board }) {
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
			// MQTT publish logic goes here
		}, 2000);
	};

	return (
		<Card
			withBorder
			shadow="sm"
			radius="md"
			key={board.id}
			mb="xs"
			padding={0}
			opacity={board.status === 0 ? 0.5 : 1}
		>
			<Accordion
				chevron={null}
				style={{ borderLeft: `3px solid ${board.leds.color}` }}
			>
				<Accordion.Item value={board.id} key={board.id}>
					<Accordion.Control aria-label={board.name}>
						<AccordionLabel board={board} />
					</Accordion.Control>
					<Accordion.Panel
						pos="relative"
						style={{ borderTop: "1px solid rgba(128,128,128,0.125)" }}
					>
						<LoadingOverlay
							visible={loading}
							zIndex={1000}
							overlayProps={{ radius: "sm", blur: 2 }}
						/>
						<SimpleGrid cols={{ base: 1, sm: 2 }} pt="xs">
							<CardSection inheritPadding>
								<SimpleGrid cols={2} spacing="sm" h="100%">
									<Card>
										<Flex
											direction="column"
											align="center"
											justify="center"
											h="100%"
										>
											<Box mb="xs">
												<IconWifi size={35} opacity={0.75} />
											</Box>
											<Text>{board.status === 1 ? "Online" : "Offline"}</Text>
											<Text size="sm" c="dimmed">
												Status
											</Text>
										</Flex>
									</Card>
									<Card ta="center">
										<Flex
											direction="column"
											align="center"
											justify="center"
											h="100%"
										>
											<Box mb="xs">
												<IconPower size={30} opacity={0.75} />
											</Box>
											<Text>{board.leds.status === 1 ? "On" : "Off"}</Text>
											<Text size="sm" c="dimmed">
												LED Strip
											</Text>
										</Flex>
									</Card>
									<Card ta="center">
										<Flex
											direction="column"
											align="center"
											justify="center"
											h="100%"
										>
											<Box mb="xs">
												<IconPalette size={30} opacity={0.75} />
											</Box>
											<Text c={board.leds.color}>{board.leds.color}</Text>
											<Text size="sm" c="dimmed">
												Color
											</Text>
										</Flex>
									</Card>
									<Card ta="center">
										<Flex
											direction="column"
											align="center"
											justify="center"
											h="100%"
										>
											<Box mb="xs">
												<IconBrightness size={30} opacity={0.75} />
											</Box>
											<Text>
												{brightnessToPercentage(board.leds.brightness)}%
											</Text>
											<Text size="sm" c="dimmed">
												Brightness
											</Text>
										</Flex>
									</Card>
								</SimpleGrid>
							</CardSection>
							<Card.Section inheritPadding>
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
									Update Settings
								</Button>
							</Card.Section>
						</SimpleGrid>
					</Accordion.Panel>
				</Accordion.Item>
			</Accordion>
		</Card>
	);
}
