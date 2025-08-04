import {
	ActionIcon,
	Button,
	Group,
	Modal,
	TextInput,
	Tooltip,
} from "@mantine/core";
import { useForm } from "@mantine/form";
import { useDisclosure } from "@mantine/hooks";
import { notifications } from "@mantine/notifications";
import { IconTag } from "@tabler/icons-react";
import { useBoardActions } from "hooks/useBoardActions";

export function ChangeNameModal({ id, name }: { id: string; name?: string }) {
	const { setBoardName } = useBoardActions();

	const [opened, { open, close }] = useDisclosure(false);

	const form = useForm({
		mode: "uncontrolled",
		initialValues: {
			name: name || "",
		},
	});

	const handleSubmit = async (values: { name: string }) => {
		try {
			await setBoardName(id, values.name);

			close();
			form.reset();

			notifications.show({
				title: "Success",
				message: "Name change command successfully sent to board",
				color: "green",
				withBorder: true,
				withCloseButton: false,
			});
		} catch (error) {
			notifications.show({
				title: "Error",
				message: "Failed to send name change command to board.",
				color: "red",
				withBorder: true,
				withCloseButton: false,
			});

			console.error("Error updating board name:", error);
		}
	};

	return (
		<>
			<Modal opened={opened} onClose={close} title="Change Name">
				<form onSubmit={form.onSubmit(handleSubmit)}>
					<TextInput
						withAsterisk
						placeholder="Enter new board name"
						key={form.key("name")}
						{...form.getInputProps("name")}
					/>

					<Group mt="xs">
						<Button fullWidth type="submit">
							Save Name
						</Button>
					</Group>
				</form>
			</Modal>

			<Tooltip label="Edit Name">
				<ActionIcon variant="default" aria-label="Edit Name" onClick={open}>
					<IconTag size={16} />
				</ActionIcon>
			</Tooltip>
		</>
	);
}
