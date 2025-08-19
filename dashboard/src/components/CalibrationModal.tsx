import {
	Button,
	Divider,
	Modal,
	NumberInput,
	Text,
	Tooltip,
} from "@mantine/core";
import { useForm } from "@mantine/form";
import { useDisclosure } from "@mantine/hooks";
import { IconBolt } from "@tabler/icons-react";
import { useBoardActions } from "hooks/useBoardActions";
import { useEffect, useRef, useState } from "react";
import { showErrorNotification, showSuccessNotification } from "src/utils";
import type { Board } from "types/board";

export function CalibrationModal({
	board: { id: boardId, name, threshold },
}: {
	board: Board;
}) {
	const { startCalibration, setCurrentThresholdOffset } = useBoardActions();
	const [opened, { open, close }] = useDisclosure();

	const [calibrating, setCalibrating] = useState(false);

	const startSnapshotRef = useRef<string | null>(null);
	const timeoutRef = useRef<number | null>(null);

	const form = useForm({
		mode: "controlled",
		initialValues: {
			offset: threshold.offset,
		},
	});

	const handleStartCalibration = async () => {
		try {
			startSnapshotRef.current = JSON.stringify(threshold);

			setCalibrating(true);

			await startCalibration(boardId);

			const successMessage = (
				<Text size="sm">
					Calibration initiated for board:
					<Text span fw={700}>{` ${name} `}</Text>
				</Text>
			);

			if (timeoutRef.current) window.clearTimeout(timeoutRef.current);
			timeoutRef.current = window.setTimeout(() => {
				setCalibrating(false);
			}, 10000);

			showSuccessNotification(successMessage);
		} catch (error) {
			setCalibrating(false);
			showErrorNotification("Failed to initiate calibration.", error.message);
		}
	};

	const handlesetCurrentThresholdOffset = async (offset: number) => {
		try {
			await setCurrentThresholdOffset(boardId, offset);

			const successMessage = (
				<Text size="sm">
					Saving threshold offset for board:
					<Text span fw={700}>{` ${name} `}</Text>
				</Text>
			);

			showSuccessNotification(successMessage);
		} catch (error) {
			showErrorNotification(
				"Failed to update threshold offset.",
				error.message,
			);
		}
	};

	useEffect(() => {
		if (!calibrating) return;

		const now = JSON.stringify(threshold);

		if (startSnapshotRef.current && now !== startSnapshotRef.current) {
			setCalibrating(false);
			if (timeoutRef.current) {
				window.clearTimeout(timeoutRef.current);
				timeoutRef.current = null;
			}

			showSuccessNotification(
				<Text size="sm">
					Calibration finished for:
					<Text span fw={700}>{` ${name} `}</Text>
				</Text>,
			);
		}
	}, [threshold, calibrating, name]);

	useEffect(() => {
		if (threshold?.offset !== undefined) {
			form.setFieldValue("offset", threshold.offset);
		}
	}, [threshold.offset, form.setFieldValue]);

	useEffect(() => {
		return () => {
			if (timeoutRef.current) {
				window.clearTimeout(timeoutRef.current);
				timeoutRef.current = null;
			}
		};
	}, []);

	return (
		<>
			<Modal opened={opened} onClose={close} title="Power Calibration">
				<Text size="sm" c="dimmed">
					The board is calibrated by putting the connected console in
					off/stand-by mode and initiating the calibration process by clicking
					the "calibrate"-button below.
				</Text>

				<Button
					fullWidth
					onClick={handleStartCalibration}
					mt="lg"
					loading={calibrating}
				>
					Calibrate
				</Button>

				<Divider mt="lg" opacity={0.75} />

				<NumberInput
					label="Baseline"
					description="Populated automatically after calibration"
					value={threshold.baseline}
					mt="md"
					disabled
				/>

				<form
					onSubmit={form.onSubmit(async ({ offset }) => {
						await handlesetCurrentThresholdOffset(offset);
					})}
				>
					<NumberInput
						label="Offset"
						description="Can be adjusted manually, but the default should work"
						{...form.getInputProps("offset")}
						mt="md"
					/>

					<Button type="submit" mt="lg" fullWidth variant="outline">
						Save Offset
					</Button>
				</form>
			</Modal>

			<Tooltip label="Start on/off calibration">
				<Button
					variant="default"
					size="xs"
					radius="xl"
					leftSection={<IconBolt size={14} />}
					onClick={open}
				>
					Calibration
				</Button>
			</Tooltip>
		</>
	);
}
