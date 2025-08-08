import { Button, Center, Stack, Text, ThemeIcon, Tooltip } from "@mantine/core";
import { IconPlugConnected, IconRefresh } from "@tabler/icons-react";
import { MqttContext } from "context/MqttProvider";
import { useContext } from "react";
import { getRuntimeConfig } from "src/runtimeConfig";

export function EmptyBoards() {
	const client = useContext(MqttContext);
	const connected = !!client && client.connected;
	const { MQTT_URL } = getRuntimeConfig();

	return (
		<Center mih="60vh">
			<Stack gap="md" align="center">
				<ThemeIcon size={56} radius="xl">
					<IconPlugConnected />
				</ThemeIcon>

				<Text fw={600} size="xl">
					No boards available
				</Text>

				<Text c="dimmed" ta="center" maw={520}>
					When a board connects to the broker, it'll show up here automatically.
					Make sure your MQTT settings are correct and the devices are powered
					on.
				</Text>

				<Button
					variant="light"
					onClick={() => window.location.reload()}
					leftSection={<IconRefresh size={18} />}
					my="md"
				>
					Retry
				</Button>

				<Tooltip label={MQTT_URL || "No MQTT URL configured"}>
					<Text size="sm" c={connected ? "teal" : "red"} fw={500}>
						MQTT Broker: {MQTT_URL || "—"} ·{" "}
						{connected ? "Connected" : "Disconnected"}
					</Text>
				</Tooltip>
			</Stack>
		</Center>
	);
}
