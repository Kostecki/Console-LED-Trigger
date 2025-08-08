import { Center, Loader, Stack, Text, Transition } from "@mantine/core";

export function LoadingState() {
	return (
		<Transition
			mounted={true}
			transition="fade"
			duration={120}
			timingFunction="ease-out"
		>
			{(styles) => (
				<Center mih="20vh" style={styles}>
					<Stack align="center" gap="xs">
						<Loader />
						<Text c="dimmed">Waiting for boards…</Text>
					</Stack>
				</Center>
			)}
		</Transition>
	);
}
