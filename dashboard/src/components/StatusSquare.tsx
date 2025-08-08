import { Box, Card, Flex, Text } from "@mantine/core";
import type { ReactNode } from "react";

export function StatusSquare({
	icon,
	label,
	value,
	color,
}: {
	icon: ReactNode;
	label: string;
	value: string;
	color?: string;
}) {
	return (
		<Card p="xs">
			<Flex direction="column" align="center" justify="center" h="100%">
				<Box mb={5}>{icon}</Box>
				<Text c={color}>{value}</Text>
				<Text size="sm" c="dimmed">
					{label}
				</Text>
			</Flex>
		</Card>
	);
}
