import {
	type BoxProps,
	Button,
	FileButton,
	Flex,
	Text,
	Tooltip,
} from "@mantine/core";
import { IconFileUpload, IconSearch } from "@tabler/icons-react";
import { useBoardActions } from "hooks/useBoardActions";
import { showErrorNotification, showSuccessNotification } from "src/utils";
import type { Board } from "../../types/board";
import { CalibrationModal } from "./CalibrationModal";
import { ChangeNameModal } from "./ChangeNameModal";

type InputProps = {
	board: Board;
} & BoxProps;

export function ActionBar({ board, ...props }: InputProps) {
	const { identifyBoard, sendFirmwareUpdate } = useBoardActions();

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
					</Text>
				);

				showSuccessNotification(successMessage);
			} catch (error) {
				showErrorNotification(
					"Failed to send firmware update command to board.",
					error.message,
				);
			}
		} catch (error) {
			showErrorNotification("Failed to upload firmware.", error.message);
			return;
		}
	};

	const handleIdentifyBoard = async () => {
		try {
			await identifyBoard(board.id);

			const successMessage = (
				<>
					<Text size="sm">
						Identify command sent to board:
						<Text span fw={700}>{` ${board.name} `}</Text>
					</Text>
					<Text fs="italic" size="sm">
						LEDs will blink for 5 seconds.
					</Text>
				</>
			);

			showSuccessNotification(successMessage);
		} catch (error) {
			showErrorNotification(
				"Failed to send identify command to board.",
				error.message,
			);
		}
	};

	return (
		<Flex
			justify="space-between"
			{...props}
			style={{
				pointerEvents: board.status === 0 ? "none" : "auto",
			}}
		>
			<ChangeNameModal board={board} />

			<Tooltip label="Flash LEDs">
				<Button
					variant="default"
					size="xs"
					radius="xl"
					leftSection={<IconSearch size={14} />}
					onClick={handleIdentifyBoard}
				>
					Identify
				</Button>
			</Tooltip>

			<CalibrationModal board={board} />

			<FileButton onChange={handleUploadFirmware} accept=".bin">
				{(props) => (
					<Tooltip label="Upload new firmware">
						<Button
							variant="default"
							size="xs"
							radius="xl"
							leftSection={<IconFileUpload size={14} />}
							{...props}
						>
							Firmware
						</Button>
					</Tooltip>
				)}
			</FileButton>
		</Flex>
	);
}
