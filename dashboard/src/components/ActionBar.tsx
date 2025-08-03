import {
	ActionIcon,
	FileButton,
	Flex,
	Group,
	Text,
	Tooltip,
} from "@mantine/core";
import { notifications } from "@mantine/notifications";
import { IconFileUpload, IconRotate, IconSearch } from "@tabler/icons-react";
import type { Board } from "../../types/board";

export function ActionBar({ board }: { board: Board }) {
	const handleUploadFirmware = (file: File) => {
		// TODO: Logic to handle firmware upload
		console.log(`Upload Firmware clicked. Board: ${board.id}`);
		console.log(`File selected: ${file.name}`);

		const message = (
			<Text>
				Firmware file
				<Text span fw={700}>
					{` ${file.name} `}
				</Text>{" "}
				sent to board
				<Text span fw={700}>
					{` ${board.name} `}
				</Text>{" "}
				({board.id}).
			</Text>
		);

		notifications.show({
			title: "Firmware Upload",
			message: message,
			color: "green",
			withBorder: true,
			autoClose: 5000,
		});
	};

	const handleIdentifyBoard = () => {
		// TODO: Logic to identify the board
		console.log(`Identify Board clicked. Board: ${board.id}`);

		const message = (
			<Text>
				Command sent to board:
				<Text span fw={700}>{` ${board.name} `}</Text>
				<Text span>({board.id}).</Text>
				<Text fs="italic">LEDs will blink for 5 seconds.</Text>
			</Text>
		);

		notifications.show({
			title: "Identify Board",
			message,
			color: "green",
			withBorder: true,
			autoClose: 5000,
		});
	};

	const handleRebootBoard = () => {
		// TODO: Logic to reboot the board
		console.log(`Reboot Board clicked. Board: ${board.id}`);

		const message = (
			<Text>
				Command sent to board:
				<Text span fw={700}>{` ${board.name} `}</Text>
				<Text span>({board.id}).</Text>
			</Text>
		);

		notifications.show({
			title: "Reboot Board",
			message,
			color: "green",
			withBorder: true,
			autoClose: 5000,
		});
	};

	return (
		<Flex justify="space-between">
			<Group gap="xs">
				<FileButton onChange={handleUploadFirmware}>
					{(props) => (
						<Tooltip label="Upload Firmware">
							<ActionIcon
								variant="default"
								aria-label="Upload Firmware"
								{...props}
							>
								<IconFileUpload
									style={{ width: "70%", height: "70%" }}
									stroke={1.5}
								/>
							</ActionIcon>
						</Tooltip>
					)}
				</FileButton>
				<Tooltip label="Identify Board">
					<ActionIcon
						variant="default"
						aria-label="Identify Board"
						onClick={handleIdentifyBoard}
					>
						<IconSearch style={{ width: "70%", height: "70%" }} stroke={1.5} />
					</ActionIcon>
				</Tooltip>
			</Group>
			<Group>
				<Tooltip label="Reboot Board">
					<ActionIcon
						variant="default"
						aria-label="Reboot Board"
						onClick={handleRebootBoard}
					>
						<IconRotate style={{ width: "70%", height: "70%" }} stroke={1.5} />
					</ActionIcon>
				</Tooltip>
			</Group>
		</Flex>
	);
}
