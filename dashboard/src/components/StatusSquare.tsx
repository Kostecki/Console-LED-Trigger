import { Box, Card, Flex, Text } from "@mantine/core";
import type { ReactNode } from "react";

export function StatusSquare({
	icon,
	label,
	value,
	color,
	fw,
}: {
	icon: ReactNode;
	label: string;
	value: string;
	color?: string;
	fw?: string;
}) {
	return (
		<Card p="xs">
			<Flex direction="column" align="center" justify="center" h="100%">
				<Box mb={5}>{icon}</Box>
				<Text c={color} fw={fw ?? "400"}>
					{value}
				</Text>
				<Text size="sm" c="dimmed">
					{label}
				</Text>
			</Flex>
		</Card>
	);
}
