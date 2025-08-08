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
import { useBoardActions } from "hooks/useBoardActions";
import type { Board } from "../../types/board";
import { ChangeNameModal } from "./ChangeNameModal";

export function ActionBar({ board }: { board: Board }) {
	const { identifyBoard, sendFirmwareUpdate, rebootBoard } = useBoardActions();

	const handleUploadFirmware = async (file: File) => {
		try {
			const res = await fetch("/api/upload", {
				method: "POST",
				headers: {
					"Content-Type": "application/octet-stream",
				},
				body: file,
			});

			if (!res.ok) {
				throw new Error(`Upload failed: ${res.statusText}`);
			}

			const { url } = await res.json();

			try {
				await sendFirmwareUpdate(board.id, url);

				const successMessage = (
					<Text size="sm">
						Firmware update command sent to board:
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
					message: "Failed to send firmware update command to board.",
					color: "red",
					withBorder: true,
					withCloseButton: false,
				});

				console.error("Error sending firmware update:", error);
			}
		} catch (error) {
			notifications.show({
				title: "Error",
				message: `Failed to upload firmware: ${error.message}`,
				color: "red",
				withBorder: true,
				withCloseButton: false,
			});

			console.error("Error uploading firmware:", error);
			return;
		}
	};

	const handleIdentifyBoard = async () => {
		try {
			await identifyBoard(board.id);

			const successMessage = (
				<Text size="sm">
					Identify command sent to board:
					<Text span fw={700}>{` ${board.name} `}</Text>
					<Text span>({board.id}).</Text>
					<Text fs="italic">LEDs will blink for 5 seconds.</Text>
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
				message: "Failed to send identify command to board.",
				color: "red",
				withBorder: true,
				withCloseButton: false,
			});

			console.error("Error identifying board:", error);
		}
	};

	const handleRebootBoard = async () => {
		try {
			await rebootBoard(board.id);

			const successMessage = (
				<Text size="sm">
					Reboot command sent to board:
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
				message: "Failed to send reboot command to board.",
				color: "red",
				withBorder: true,
				withCloseButton: false,
			});

			console.error("Error rebooting board:", error);
		}
	};

	return (
		<Flex justify="space-between">
			<Group gap="xs">
				<ChangeNameModal id={board.id} name={board.name} />
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
			<Group gap="xs">
				<FileButton onChange={handleUploadFirmware} accept=".bin">
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
