import {
	Accordion,
	Card,
	CardSection,
	Divider,
	Group,
	SimpleGrid,
	Stack,
	Text,
} from "@mantine/core";
import {
	IconBulb,
	IconLamp,
	IconPalette,
	IconPower,
	IconWifi,
} from "@tabler/icons-react";
import { useNow } from "context/NowContext";
import { dayjs } from "lib/dayjs";
import type { Board, OnlineStatus } from "../../types/board";
import { brightnessToPercentage, displayColor } from "../utils";
import { ActionBar } from "./ActionBar";
import { ColorBrightnessPicker } from "./ColorBrightnessPicker";
import { StatusSquare } from "./StatusSquare";

const slightBorderColor = "rgba(128, 128, 128, 0.125)";
const slightBorder = `1px solid ${slightBorderColor}`;

const showLedStatus = (ledsStatus: OnlineStatus) => {
	let opacity = 0.3;
	let color = "#000000"; // Black for "OFF"

	if (ledsStatus === 1) {
		opacity = 0.75;
		color = "#FFB300"; // Yellow color for "ON"
	}

	return <IconBulb size={24} color={color} opacity={opacity} />;
};

// Format boot time into a human-readable string
// Example: "Uptime: 1d 2h 30m"
const formatBootTime = (bootTime: number, nowUnixMs: number) => {
	const boot = dayjs.unix(bootTime);
	const now = dayjs(nowUnixMs);

	const diffMs = now.diff(boot);
	if (diffMs < 0) return "Uptime: -";

	const diff = dayjs.duration(diffMs);
	const days = Math.floor(diff.asDays());
	const hours = diff.hours();
	const minutes = diff.minutes();

	const parts: string[] = [];
	if (days > 0) parts.push(`${days}d`);
	if (hours > 0 || days > 0) parts.push(`${hours}h`);
	parts.push(`${minutes}m`);

	return `Uptime: ${parts.join(" ")}`;
};

const AccordionLabel = ({ board }: { board: Board }) => {
	const now = useNow();

	return (
		<Group justify="space-between" align="center">
			<Stack gap={0}>
				<Text fw={500}>{board.name}</Text>
				<Text size="xs" fw={400} c="dimmed" fs="italic">
					{`${board.id} | ${
						board.status === 1 ? formatBootTime(board.bootTime, now) : "Offline"
					}`}
				</Text>
			</Stack>
			{showLedStatus(board.leds.status)}
		</Group>
	);
};

export function ConsoleCard({ board }: { board: Board }) {
	const boardColor = displayColor(board);

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
				style={{ borderLeft: `3px solid ${boardColor}` }}
			>
				<Accordion.Item value={board.id} key={board.id}>
					<Accordion.Control aria-label={board.name}>
						<AccordionLabel board={board} />
					</Accordion.Control>
					<Accordion.Panel style={{ borderTop: slightBorder }}>
						<ActionBar board={board} mt={5} mb="sm" />
						<Divider color={slightBorderColor} />
						<SimpleGrid cols={{ base: 1, sm: 2 }} pt="xs">
							<CardSection inheritPadding>
								<SimpleGrid cols={1} spacing={0} h="100%">
									<SimpleGrid cols={2} spacing="sm">
										<StatusSquare
											icon={<IconWifi size={35} opacity={0.75} />}
											label="Status"
											value={board.status === 1 ? "Online" : "Offline"}
										/>
										<StatusSquare
											icon={<IconPower size={30} opacity={0.75} />}
											label="LED Strip"
											value={board.leds.status === 1 ? "On" : "Off"}
										/>
										<StatusSquare
											icon={<IconPalette size={30} opacity={0.75} />}
											label="Color"
											value={boardColor.toUpperCase()}
											color={boardColor}
											fw="500"
										/>
										<StatusSquare
											icon={<IconLamp size={30} opacity={0.75} />}
											label="Brightness"
											value={`${brightnessToPercentage(board.leds.brightness)}%`}
										/>
									</SimpleGrid>
								</SimpleGrid>
							</CardSection>
							<Card.Section inheritPadding>
								<ColorBrightnessPicker board={board} />
							</Card.Section>
						</SimpleGrid>
					</Accordion.Panel>
				</Accordion.Item>
			</Accordion>
		</Card>
	);
}
